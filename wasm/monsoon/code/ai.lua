ai = {}

function ai.TriangleWave(self)
    if not self.init then
        self.init = true
        self.counter = vx.Random(0, 29)
        self.x_inc = ((self.direction == direction.Left) and -1 or 1)
        self.y_inc = (1 - (2 * vx.Random(0, 1)))
    end
    
    self.counter = self.counter + 1

    if self.x_speed == 0 then
        self.direction = self.direction.opposite
        self.x_inc = ((self.direction == direction.Left) and -1 or 1)
    end
    self.x_speed = self.x_inc
    
    if self.counter % 40 == 0 then
        self.y_inc = self.y_inc * -1
    end
    self.y_speed = self.y_inc
end

function ai.StandAndStare(self)
    if not self.init then
        self.init = true
    end
    
    if player.x < self.x - 16 then
        self.direction = direction.Left
    elseif player.x > self.x + self:GetHotspot('main').width + 16 then
        self.direction = direction.Right
    end
    
    if not self:LocateGround() then
        self:ApplyGravity()
    end
end

