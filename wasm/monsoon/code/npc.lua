vergeclass 'NPC'(Entity) do
    function NPC:__init(...)
        super(...)
        
        self.stance = 'idle'
        self.meta = nil
        self.hostile = true
        self.max_health = 1
        self.health = self.max_health
        self.stun_timer = 0
    end

    function NPC:ParseMeta()
        self.direction = self.meta.direction or direction.Left
        self.investigate_action = self.meta.investigate_action
        self.max_health = self.meta.health or 1
        self.health = self.max_health
    end
    
    function NPC:Damage(amount)
        if self.health > 0 then
            self.health = self.health - amount
            if self.health <= 0 then
                resources.sounds.stun_enemy:Play()
                self.health = 0
                self.tint = vx.RGB(0, 255, 255)
                self.stun_timer = 200
            else
                if self.meta and self.meta.hit_callback then
                    self.meta.hit_callback(self)
                else
                    resources.sounds.hurt_enemy:Play()
                end
            end
            return true
        end
        return false
    end
    
	local get_xwin = v3.get_xwin
	local get_ywin = v3.get_ywin
    function NPC:Render()
		if not self.visible then
			return
		end
        Sprite.Render(self)
        
        if flags[FLAG_ITEM_BOTTLE] > 0 and self.stun_timer > 0 then
            local x = self.x - get_xwin()
            local y = self.y - get_ywin()
            local hotspot = self:GetHotspot('main')
            local sine = math.sin(math.rad(vx.clock.systemtime * 10))
            local radius = math.max(hotspot.width, hotspot.height) / 2 + 6 + (sine * 2)
            local b =  sine * 64 + 191
            
            for i = 0, 2 do
                vx.screen:Circle(x + hotspot.width / 2, y + hotspot.height / 2, radius - (i * 2), radius - (i * 2), vx.RGB(0, b / (i + 1), b / (i + 1)))
            end
        end
    end

    function NPC:Update()
        if not self.visible then
            return
        end
    
        if self.meta and self.meta.animation_override_time and self.meta.animation_override_time > 0 then
            self.meta.animation_override_time = self.meta.animation_override_time - 1
        else
            self.stance = 'idle'
        end
        
        if self.health == 0 then
            self.x_speed = 0
            self.y_speed = 0
            self.stun_timer = self.stun_timer - 1
            
            -- Player captures this NPC
            if flags[FLAG_ITEM_BOTTLE] > 0 and self:Touches(player) then
                self.visible = false
                resources.sounds.capture_npc:Play()
                GetCapture(self.meta.capture)
            end
            
            if self.stun_timer == 0 then
                self.health = self.max_health
                self.tint = false
            end
        elseif self.meta and self.meta.update_callback then
            self.meta.update_callback(self)
            if self:Touches(player) and self.investigate_action then
                InvestigateCheck(self.investigate_action)
            end
        end
        
        -- Update the entity
        Entity.Update(self)
        
        -- Alter animation shown.
        self:SetAnimation(self.stance)
    end
end
