SHOW_PARTICLES = false

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
        self.particle_count = tonumber(bullet_info.particle_count)
        self.particle_type = tostring(bullet_info.particle_type)
        self.lucent = tonumber(bullet_info.lucent) or 0
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
function BulletAction.Particle(self)
    if not self.init then
        self.time = 500
        self.init = true
    end
    
    self.time = self.time - 1
    if self.time == 0 then
        if self.death_sound then
            self.death_sound:Play()
        end
        self.dispose = true
        return
    end
    self.x = self.x + math.cos(self.angle) * self.bullet.speed
    self.y = self.y + math.sin(self.angle) * self.bullet.speed
end

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
        self:DisplaceDiamond()
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
        self.lucent = bullet.lucent
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
    
    local HEART = 80
    local DIAMOND = 95
    function Projectile:DestroyHeart()
        local hotspot = self:GetHotspot('main')
        local x = math.floor(self.x / 16)
        local x2 = math.floor((self.x + hotspot.width) / 16)
        local y = math.floor(self.y / 16)
        local y2 = math.floor((self.y + hotspot.height) / 16)
        if vx.map:GetTile(x, y, 1) == HEART then
            DestroyTile(x, y)
        elseif vx.map:GetTile(x, y2, 1) == HEART then
            DestroyTile(x, y2)
        elseif vx.map:GetTile(x2, y, 1) == HEART then
            DestroyTile(x2, y)
        elseif vx.map:GetTile(x2, y2, 1) == HEART then
            DestroyTile(x2, y2)
        end
    end
    
    
    local function near(a, b)
        return math.abs(a - b) % (2 * math.pi) < 0.1
    end
    
    local function DisplaceTile(x, y, direction)
        if vx.map:GetTile(x, y, 1) == DIAMOND then
            if near(direction, 0) then
                if vx.map:GetObs(x + 1, y) == 0 then
                    player.x = (x + 1) * 16
                    player.y = y * 16 + 1
                else
                    player.x = (x - 1) * 16
                    player.y = y * 16 + 1
                end
            elseif near(direction, math.pi / 2) then
                if vx.map:GetObs(x, y - 1) == 0 then
                    player.x = x * 16
                    player.y = (y - 1) * 16 + 1
                else
                end 
            elseif near(direction, math.pi) then
                if vx.map:GetObs(x - 1, y) == 0 then
                    player.x = (x - 1) * 16
                    player.y = y * 16 + 1
                else
                    player.x = (x + 1) * 16
                    player.y = y * 16 + 1
                end
            elseif near(direction, math.pi * 3 / 2) then
                if vx.map:GetObs(x, y + 1) == 0 then
                    player.x = x * 16
                    player.y = (y + 1) * 16 + 1
                else
                end
            end
            return true
        end
        return false
    end
    
    function Projectile:DisplaceDiamond()
        local hotspot = self:GetHotspot('main')
        local x = math.floor(self.x / 16)
        local x2 = math.floor((self.x + hotspot.width) / 16)
        local y = math.floor(self.y / 16)
        local y2 = math.floor((self.y + hotspot.height) / 16)
        if DisplaceTile(x, y, self.angle)
            or DisplaceTile(x, y2, self.angle)
            or DisplaceTile(x2, y, self.angle)
            or DisplaceTile(x2, y2, self.angle)
        then
        end
    end
    
    function Projectile:FindTarget()
        for i, s in ipairs(sprites) do
            if s.can_target and s ~= self and self.target_hostile == s.hostile and s.visible then
                if self:Touches(s) then
                    return s
                end
            end
        end
        return nil
    end
    
    function Projectile:Update()
        self.visible = true
        if not camera:ContainsPoint(self.x, self.y) then
            self.dispose = true
        end
        self.bullet.move(self)
        if self.dispose and SHOW_PARTICLES then
            if self.bullet.particle_count then
                for i = 1, self.bullet.particle_count do
                    Projectile(self.x, self.y, self.angle + math.random() * 2 * math.pi, bullets[self.bullet.particle_type], 0, false)
                end
            end
        end
        
        Sprite.Update(self)
    end
end
