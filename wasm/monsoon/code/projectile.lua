vergeclass 'Bullet' do
    function Bullet:__init(name, bullet_info)
        self.name = name
        self.filename = bullet_info.filename
        if not bullet_info.move or not BulletAction[bullet_info.move] then
            error('Failed to initialize bullet type because movement callback BulletAction.' .. bullet_info.move .. '() does not exist', 2)
        end
        self.move = BulletAction[bullet_info.move]
        self.speed = tonumber(bullet_info.speed)
        self.damage = tonumber(bullet_info.damage)
    end
end

function InitBullets()
    local bullet_entries, err = MungoReadTable('resources/data/bullet.mungo')
    if err then
        vx.Exit(err)
    end
    
    bullets = {}
    for bullet_name, bullet_info in pairs(bullet_entries) do
        bullets[bullet_name] = Bullet(bullet_name, bullet_info)
    end
end

BulletAction = {}
function BulletAction.Simple(self)
    local target = self:FindTarget()
    
    if target and target:Damage(self.bullet.damage) then
        self.dispose = true
        return
    elseif self:InObstruction() then
        if self.death_sound then
            self.death_sound:Play()
        end
        self:DestroyHeart()
        self.dispose = true
        return
    end
    self.x = self.x + math.cos(self.angle) * self.bullet.speed
    self.y = self.y + math.sin(self.angle) * self.bullet.speed
end


vergeclass 'Projectile'(Sprite) do
    function Projectile:__init(x, y, directional_arg, bullet, wait, target_hostile)
        super(x, y, bullet.filename)
        
        if type(directional_arg) == 'number' then
            local closest = nil
            self.angle = directional_arg
            for dir_name, dir in pairs(direction) do
                if not closest or math.abs(dir.angle - self.angle) < math.abs(closest.angle - self.angle) then
                    closest = dir
                end
            end
            self.direction = closest
        elseif type(directional_arg) == 'table' then
            self.direction = directional_arg
            self.angle = self.direction.angle
        end
        self.can_target = false
        self.visible = false
        self.target_hostile = target_hostile
        self.bullet = bullet
        self:SetAnimation('move')
    end
    
    function Projectile:InObstruction()
        local hotspot = self:GetHotspot('main')
        local x = self.x
        local x2 = x + hotspot.width
        local y = self.y
        local y2 = y + hotspot.height
        return vx.map:GetObsPixel(x, y) or vx.map:GetObsPixel(x, y2) or vx.map:GetObsPixel(x2, y) or vx.map:GetObsPixel(x2, y2)
    end
    
    function Projectile:DestroyHeart()
        local heart = 80
        local hotspot = self:GetHotspot('main')
        local x = (self.x) / 16
        local x2 = (self.x + hotspot.width) / 16
        local y = (self.y) / 16
        local y2 = (self.y + hotspot.height) / 16
        if vx.map:GetTile(x, y, 1) == heart then
            DestroyTile(x, y)
        elseif vx.map:GetTile(x, y2, 1) == heart then
            DestroyTile(x, y2)
        elseif vx.map:GetTile(x2, y, 1) == heart then
            DestroyTile(x2, y)
        elseif vx.map:GetTile(x2, y2, 1) == heart then
            DestroyTile(x2, y2)
        end
    end
    
    function Projectile:FindTarget()
        for i, s in ipairs(sprites) do
            if s.can_target and s ~= self and self.target_hostile == s.hostile then
                if self:Touches(s) then
                    return s
                end
            end
        end
        return nil
    end
    
    function Projectile:Update()
        self.visible = true
        if self.x < vx.camera.x - 10
            or self.x > vx.camera.x + vx.screen.width + 10
            or self.y < vx.camera.y - 10
            or self.y > vx.camera.y + vx.screen.height + 10 then
                self.dispose = true
        end
        self.bullet.move(self)
        Sprite.Update(self)
    end
end
