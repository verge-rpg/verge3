vergeclass 'Camera' do
    local CAMERA_DELAY = 25
    function Camera:__init()
        self.target = nil
        self.free = false
        self.can_move_free = false
        self.smooth_transition = true

        self.dest_x = nil
        self.dest_y = nil
        self.delay = 0
        
        self.x = vx.camera.x
        self.y = vx.camera.y
    end
    
    function Camera:SetTarget(target)
        self.dest_x = math.floor((target.x + 8) / vx.screen.width) * vx.screen.width
        self.dest_y = math.floor((target.y + 8) / vx.screen.height) * vx.screen.height
        self.delay = 0
        
        self.x = self.dest_x
        self.y = self.dest_y
        vx.camera.x = self.x
        vx.camera.y = self.y
        
        self.init = true
        self.target = target
    end
    
    function Camera:ContainsPoint(x, y)
        if self.delay > 0 then
            return (x >= self.x - 10
                    and x <= self.x + vx.screen.width + 10
                    and y >= self.y - 10
                    and y <= self.y + vx.screen.height + 10)
                or (x >= self.dest_x - 10
                    and x <= self.dest_x + vx.screen.width + 10
                    and y >= self.dest_y - 10
                    and y <= self.dest_y + vx.screen.height + 10)
        else
            return x >= self.x - 10
                and x <= self.x + vx.screen.width + 10
                and y >= self.y - 10
                and y <= self.y + vx.screen.height + 10 
        end
    end
    
    function Camera:Update()
        if self.target then
            local target = self.target
            
            if self.free then
                self.dest_x = (target.x + 8) - vx.screen.width / 2
                self.dest_y = (target.y + 8) - vx.screen.height / 2
            else
                self.dest_x = math.floor((target.x + 8) / vx.screen.width) * vx.screen.width
                self.dest_y = math.floor((target.y + 8) / vx.screen.height) * vx.screen.height
                if self.delay == 0 then
                    self.delay = CAMERA_DELAY
                end
            end
            
            if self.delay > 0 then
                self.delay = self.delay - 1
            end
            
            if self.smooth_transition then
                if self.x < self.dest_x then
                    self.x = math.min(self.x + 4, self.dest_x)
                elseif self.x > self.dest_x then
                    self.x = math.max(self.x - 4, self.dest_x)
                end
                
                if self.y < self.dest_y then
                    self.y = math.min(self.y + 4, self.dest_y)
                elseif self.y > self.dest_y then
                    self.y = math.max(self.y - 4, self.dest_y)
                end
                
                if self.x == self.dest_x and self.y == self.dest_y then
                    self.smooth_transition = false
                end
            else
                if self.delay == 0 then
                    self.x = self.dest_x
                    self.y = self.dest_y
                end
            end
        end
        
        vx.camera.x = self.x
        vx.camera.y = self.y
    end
end

camera = Camera()