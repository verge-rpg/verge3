vergeclass 'Player'(Entity) do
    local JUMP_MAX_HEIGHT = 64
    local JUMP_START_SPEED = 2.0
    local JUMP_END_SPEED = 0.5
    local JUMP_DECEL = 0.005
    
    local MOVEMENT_MAX_SPEED = 1.5
    local LAND_ACCELERATION = 0.05
    local LAND_FRICTION = 0.90
    local AIR_ACCELERATION = 0.015
    
    
    function Player:__init(...)
        super(...)
        
        self.bounce_timer = 0
        self.invincibility_timer = 0
        
        -- Typical setup stuff
        self.lives = 1
        self.max_lives = 9
        self.moving = false
        self.can_jump = false
        self.jump = false
        self.fall = false
        self.climb_slopes = true
        self.fall_duration = 0
        self.jump_height = 0
        
        self.jump_speed = 0
        self.start_jump_speed = JUMP_START_SPEED
        self.end_jump_speed = JUMP_END_SPEED
        self.max_jump_height = JUMP_MAX_HEIGHT
        
        -- Attack related stuff
        self.weapon_ability = nil
        self.weapon_sprite = nil

        self.cooldown = 0
        self.cooldown_animation = nil
        self.projectiles = {}
        
        self.stance = 'idle'
        self:SetAnimation(self.stance)
        self.animation:Parse()
        self.z_index = 1
    end
    
    function Player:ResetInvincibility()
        self.invincibility_timer = 100
    end
    
    function Player:Die()
        -- Can't be hurt if still stunned.
        if self.invincibility_timer > 0 then
            return false
        end
        
        self.lives = self.lives - 1
        self:ResetInvincibility()
        
        -- If you're out of lives, game over!
        if self.lives <= 0 then
        end
        
        return true
    end
    
    function Player:Update()
        if self.invincibility_timer > 0 then
            self.visible = math.floor(self.invincibility_timer / 5) % 2 == 0
            self.invincibility_timer = self.invincibility_timer - 1
            if self.invincibility_timer == 0 then
                self.visible = true
            end
        end
        
        self.stance = 'idle'
        self.moving = false
        
        self:HandleWalk()
        self:HandleAir()

        if not textbox_active then
            -- Process interactions
            self:HandleZone()
            --self:HandleInvestigate()

            -- Process weapon and attack related junk!
            self:HandleWeapon()
        end
        
        -- Update the entity
        Entity.Update(self)
        
        -- Alter animation shown.
        self:SetAnimation(self.stance)
    end
    
    -- For restoring the update function.
    Player.DefaultUpdate = Player.Update
    
    function Player:HandleWalk()
        -- If we press left, walk
        if not textbox_active and button.Left.pressed then
            self.stance = 'walk'
            self.direction = direction.Left
            self.moving = true
            self.x_speed = math.max(self.x_speed - LAND_ACCELERATION, -MOVEMENT_MAX_SPEED)
        -- If we press right, walk
        elseif not textbox_active and button.Right.pressed then
            self.stance = 'walk'
            self.direction = direction.Right
            self.moving = true
            self.x_speed = math.min(self.x_speed + LAND_ACCELERATION, MOVEMENT_MAX_SPEED)
        -- Stopped walking, apply friction
        else
            self.x_speed = self.x_speed * LAND_FRICTION
            if math.abs(self.x_speed) > 0 then
                self.moving = true
            end
        end
    end
    
    function Player:HandleAir()
        self:HandleJump()
        self:HandleFall()

        -- Bumped the ceiling!
        if self:LocateCeiling() and self.bounce_timer < 0 and (self.jump or self.fall) then
            -- Check if it was a save block.
            if not self:HandleSaveBlock() then
                -- Otherwise, make a bouncing sound.
                resources.sounds.ceiling_bounce:Play()
            end
            self.bounce_timer = 10
        end
        
        self.bounce_timer = self.bounce_timer - 1
        
        -- Stop saying we're 'gripping' floor once we're in mid-air
        if self.fall or self.jump then
            self.grip_floor = false
        -- Play a sound when we first land on the floor and then
        -- from that point on, remember we're gripped until 
        -- we jump
        elseif not self.grip_floor then
            self.grip_floor = true
            if self.fall_duration > 20 then
                resources.sounds.land:Play()
            end
            self.fall_duration = 0
        end
    end
    
    function Player:HandleJump()
        if not self.can_jump or textbox_active then
            self.jump = false
            return
        end
        
        -- If player just pressed the jump button, initiate jump.
        if button.Jump.pressed and not self.jump and not self.fall then
            self:PrepareJump()
        -- If player is holding the jump button, sustain jump.
        elseif button.Jump.pressed and self.jump then
            self.stance = 'jump'
            self:SustainJump()
        -- Cancel jump, player just released the jump button
        elseif not button.Jump.pressed and self.jump then
            self.jump = false
            self.fall = true
        end
    end
    
    function Player:PrepareJump()
        self.jump_height = self.max_jump_height
        self.jump_speed = self.start_jump_speed
        self.jump = true
        self.grip_floor = false
        resources.sounds.jump:Play()
    end
    
    function Player:SustainJump()
        if self.jump_speed > self.end_jump_speed then
            self.jump_speed = math.max(self.jump_speed - JUMP_DECEL, self.end_jump_speed)
        end
    
        --local jump_speed = math.min(self.jump_height, self.jump_speed)
        local jump_speed = self.jump_speed

        self.y_speed = -jump_speed
        self.jump_height = self.jump_height - jump_speed
        if self.jump_height <= 0 or not self.jump or self:LocateCeiling() then
            self.jump_speed = 0
            self.jump = false
            self.fall = true
            button.Jump.pressed = false
        end
    end
    
    function Player:HandleFall()
        -- Not jumping, no ground beneath us, fall.
        if not self:LocateGround() and not self.jump then
            self.stance = 'fall'
            self.fall = true
            self.fall_duration = self.fall_duration + 1
        -- Ground beneath us, stop falling
        else
            self.fall = false
        end

        -- Falling, gradually increasse fall speeds.
        if self.fall then
            self:ApplyGravity()
        end
    end
    
    -- Weapon related updating
    function Player:HandleWeapon()               
        -- If we press attack, do the appropriate attack processing.
        if button.Attack.pressed and self.weapon_ability and self.cooldown == 0 then
            button.Attack.pressed = false
            local act = self.weapon_ability.action
            if act.sound then
                act.sound:Play()
            end
            self.cooldown = act.cooldown
            if self.weapon_sprite then
                if act.animation then
                    self.weapon_sprite:SetAnimation(act.animation)
                end
            end
            self.cooldown_animation = act.cooldown_animation
            for i, projectile in pairs(act.projectile) do
                table.insert(self.projectiles, { projectile, projectile.wait })
            end
        end
        
        -- Iterate over projectiles if we have any
        if #self.projectiles > 0 then
            self:HandleProjectiles()
        end
        
        -- While cooling down/performing an attack, show the
        -- attack's animation. Overrides all other animations (for now).
        if self.cooldown > 0 then
            self.cooldown = self.cooldown - 1
            self.stance = self.cooldown_animation or self.stance
            if self.cooldown == 0 then
                self.cooldown_animation = nil
            end
        end
    end

    function Player:HandleSaveBlock()
        local block = 81
        local hotspot = self:GetHotspot('main')
        local x = (self.x) / 16
        local x2 = (self.x + hotspot.width) / 16
        local y = (self.y) / 16 - 1
        if vx.map:GetTile(x, y, 1) == block then
            SaveBlock(x, y)
            return true
        elseif vx.map:GetTile(x2, y, 1) == block then
            SaveBlock(x2, y)
            return true
        end
        return false
    end

    function Player:HandleZone()
        for i = 0, 1 do
            local x = self.x / 16 + i
            local y = (self.y + 8) / 16
            local z = vx.map:GetZone(x, y)
            if z > 0 then
                name = vx.map.zone[z].name
                if name:find('<') then
                    local t = {}
                    name:gsub('[^<>(),]+', function(item) table.insert(t, item) end)
                    local x, y, filename = unpack(t)
                    MapSwitch(tonumber(x), tonumber(y), filename)
                    --vx.Exit('(' .. x .. ', ' .. y .. ') in "' .. filename .. '"')
                else
                    vx.CallFunction(vx.map.zone[z].event, {x = x, y = y, zone = z})
                end
            end
        end
    end
    
    --[[function Player:HandleInvestigate()
        -- If we press up, activate things
        if button.Up.pressed then
            for idx, s in ipairs(sprites) do
                if player:Touches(s) and s.investigate_action then
                    if s.health > 0 then
                        s:investigate_action()
                        break
                    end
                end
            end
            button.Up.pressed = false
        end
    end]]

    function Player:HandleProjectiles()
        local idx = 1
        while idx <= #self.projectiles do
            proj = self.projectiles[idx]
            if proj[2] == 0 then
                local projectile = proj[1]
                local hotspot = self:GetHotspot('main')
                local head = self:GetHotspot('head')
                local x = self.x - hotspot.x + head.x + projectile.x_offset + (self.direction == direction.Right and -4 or (-head.width - 10))
                local y = self.y - hotspot.y + head.y + head.height / 2 + projectile.y_offset
                Projectile(x, y, self.direction, projectile.bullet, projectile.wait, true)                
                table.remove(self.projectiles, idx)
            else
                proj[2] = proj[2] - 1
                idx = idx + 1
            end
        end
    end
end
