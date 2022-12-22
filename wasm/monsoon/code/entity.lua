vergeclass 'Entity'(Sprite) do
    -- The steepest slope in pixels that an entity can walk up.
    local MAXIMUM_SLOPE = 4
    
	function Entity:__init(...)
		super(...)
		self.x_speed = 0
		self.y_speed = 0
        self.climb_slopes = false
        self.descending = false
		self.grip_floor = false
        self.can_target = true
        self.hostile = false
	end
	
	function Entity:Update()
		if self.x_speed < 0 then
			self:MoveLeft()
		elseif self.x_speed > 0 then
			self:MoveRight()
		end
		
		if self.y_speed < 0 then
			self:MoveUp()
		elseif self.y_speed > 0 then
			self:MoveDown()
		end
		
		self:PreventFloorClipping()
		Sprite.Update(self)
	end

	function Entity:GetHorizontalSlopeObs(x, y)
        local slope = self.climb_slopes and MAXIMUM_SLOPE or 0
		return vx.map:GetObsPixel(math.floor(x), math.floor(y - slope) - slope + self:GetHotspot('main').height)
	end

	function Entity:ApplyGravity()
		self.y_speed = math.min(self.y_speed + GRAVITY, TERMINAL_VELOCITY)
	end

	function Entity:LocateGround()
		local hotspot = self:GetHotspot('main')
		local x = self.x
		local y = self.y + hotspot.height + 1
		return (vx.map:GetObsPixel(x, y)
			or vx.map:GetObsPixel(hotspot.width / 2 + x, y)
			or vx.map:GetObsPixel(hotspot.width + x, y))
	end
	
	function Entity:LocateCeiling()
		local hotspot = self:GetHotspot('main')
		local x = self.x
		local y = self.y - 1
		return (vx.map:GetObsPixel(x, y)
            or vx.map:GetObsPixel(hotspot.width / 2 + x, y)
			or vx.map:GetObsPixel(hotspot.width + x, y))
	end

	function Entity:PreventFloorClipping()
		prevent_clip = 0
		while self:LocateGround() and self.y > 0 do
			self.y = self.y - 1
			prevent_clip = prevent_clip + 1
		end
		if prevent_clip > 0 then
			self.y = self.y + 1
            if self.grip_floor and prevent_clip > 1 and not self.descending then
                self.x_speed = self.x_speed * 0.93
            end
		end
	end

	function Entity:DescendHorizontalSlope()
		-- Attempts to descend any sloped hills, in order to prevent falling as a result of moving quickly horizontally.
		if not self.grip_floor or not self.climb_slopes then
			return
		end
        self.descending = false
		if not self:LocateGround() then
			self.y = self.y + MAXIMUM_SLOPE
			if not self:LocateGround() then
				self.y = self.y - MAXIMUM_SLOPE
            else
                self.descending = true
			end
		end
	end

	function Entity:MoveLeft()
		local hotspot = self:GetHotspot('main')
		
		-- Move left a bit, according to our speed.
		local speed = self.x_speed
		while speed < 0 do
			local x = math.floor(self.x) - 1
			
			-- Move if we can climb a slope,
			-- or as long we're not running into a wall or the map's edge.
			if not vx.map:GetObsPixel (x, math.floor(self.y))
				and not vx.map:GetObsPixel (x, math.floor(self.y) + (hotspot.height / 2))
				and not self:GetHorizontalSlopeObs(x, self.y) then
				if speed <= -1 then
					self.x = self.x - 1
					speed = speed + 1
				else
					self.x = self.x + speed
					speed = 0
				end
				self:DescendHorizontalSlope()
			-- Otherwise, stop.
			else
				self.x_speed = 0
				return
			end
		end
	end
	
	function Entity:MoveRight()
		local hotspot = self:GetHotspot('main')
		
		-- Move right bit, according to our speed.
		local speed = self.x_speed
		while speed > 0 do
			local x = math.floor(self.x) + hotspot.width + 1
			
			-- Move if we can climb a slope,
			-- or as long we're not running into a wall or the map's edge.
			if not vx.map:GetObsPixel (x, math.floor(self.y))
				and not vx.map:GetObsPixel (x, math.floor(self.y) + (hotspot.height / 2))
				and not self:GetHorizontalSlopeObs(x, self.y) then
				if speed >= 1 then
					self.x = self.x + 1
					speed = speed - 1
				else
					self.x = self.x + speed
					speed = 0
				end
				self:DescendHorizontalSlope()
			-- Otherwise, stop.
			else
				self.x_speed = 0
				return
			end
		end
	end
	
	function Entity:MoveUp()
		local hotspot = self:GetHotspot('main')
	
		-- Move up a bit, according to our speed.
		local speed = self.y_speed
		while speed < 0 do
			local y = math.floor(self.y) - 1
			
			-- Move if we can climb a slope,
			-- or as long we're not running into a wall or the map's edge.
			if not self:LocateCeiling() then
				if speed <= -1 then
					self.y = self.y - 1
					speed = speed + 1
				else
					self.y = self.y + speed
					speed = 0
				end
			-- Otherwise, stop.
			else
				self.y_speed = 0
				return
			end
		end
	end
	
	function Entity:MoveDown()
		local hotspot = self:GetHotspot('main')
	
		-- Move up a bit, according to our speed.
		local speed = self.y_speed
		while speed > 0 do
			local y = math.floor(self.y) + hotspot.height + 1
			
			-- Move if we can climb a slope,
			-- or as long we're not running into a wall or the map's edge.
			if not vx.map:GetObsPixel (math.floor(self.x), y)
				and not vx.map:GetObsPixel (math.floor(self.x) + (hotspot.width / 2), y) then
				if speed >= -1 then
					self.y = self.y + 1
					speed = speed - 1
				else
					self.y = self.y + speed
					speed = 0
				end
			-- Otherwise, stop.
			else
				self.y_speed = 0
				return
			end
		end
	end
end
