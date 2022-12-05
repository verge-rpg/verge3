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
                self.tint = vx.RGB(255, 255, 255)
                self.stun_timer = 200
            else
                resources.sounds.hurt_enemy:Play()
            end
            return true
        end
        return false
    end

    function NPC:Update()
        self.stance = 'idle'
        
        if self.health == 0 then
            self.x_speed = 0
            self.y_speed = 0
            self.stun_timer = self.stun_timer - 1
            if self.stun_timer == 0 then
                self.health = self.max_health
                self.tint = false
            end
        elseif self.meta and self.meta.update_callback then
            self.meta.update_callback(self)
        end
        
        -- Update the entity
        Entity.Update(self)
        
        -- Alter animation shown.
        self:SetAnimation(self.stance)
    end
end
