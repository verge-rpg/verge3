vergeclass 'AnimationScript' do
	function AnimationScript:__init()
		self:Reset()
	end
	
	function AnimationScript:Reset()
		self.script = ''
		self.visible = true
		self.frame = 0
		self.wait = 0
		self.offset = 1
	end
	
	function AnimationScript:SetScript(script)
		self.script = script:upper()
	end
	
	function AnimationScript:ParseNumber()
		local value_string = ''
		local c = self.script:sub(self.offset, self.offset)
		while c >= '0' and c <= '9' and self.offset < #self.script do
			value_string = value_string .. c
			self.offset = self.offset + 1
			c = self.script:sub(self.offset, self.offset)
		end
		return num(value_string)
	end
	
	function AnimationScript:Parse()
		-- If the script is zero-length, there's nothing to do.
		if #self.script == 0 then
			return
		end
		
		-- While our animation is waiting, hold off further parsing.
		if self.wait > 0 then
			self.wait = self.wait - 1
		else
			-- Examine a character from the script string.
			c = self.script:sub(self.offset, self.offset)
			-- Switch frames.
			if c == 'F' then
				self.offset = self.offset + 1
				self.frame = self:ParseNumber()
			-- Wait a certain period of time
			elseif c == 'W' then
				self.offset = self.offset + 1
				self.wait = self:ParseNumber()
			-- Hide this entity
			elseif c == 'H' then
				self.visible = False
			-- Show this entity
			elseif c == 'S' then
				self.visible = True
			-- Go back to the beginning of the script.
			elseif c == 'B' then
				self.offset = 1
			end
			-- If the end of the script is reached, clear the script.
			if self.offset > #self.script then
				self.script = ''
			end
		end
	end
end


vergeclass 'Sprite' do
    local sprite_cache = {}
    local sprite_image_cache = {}
    local sprite_flipped_image_cache = {}

	-- Ensure that all names can be indexed internally by their lowercase name,
	-- effectively making the initialization of all key mappings case insensitive
	local function TableKeysToLowerCase(t)
		if t == nil then
			return
		end
		for k, v in pairs(t) do
			if k ~= k:lower() then
				sprite_info[k:lower()] = v
			end
			if type(v) == 'table' then
				TableKeysToLowerCase(v)
			end
		end
	end

	function Sprite:__init(...)
		t = {...}
		if type(t[1]) == 'number' and type(t[2]) == 'number' and type(t[3]) == 'string' then
			local x, y, filename = unpack(t)
			self.x = x
			self.y = y
			self.filename = filename
		else
			error("Constructor Sprite(" .. IndexedTypesToString(t) .. ") is not defined.", 2)
		end
		
		local sprite_info, err
		if not sprite_cache[self.filename] then
			sprite_info, err = MungoReadTable(self.filename)
			if err then
				error('Failed to load sprite. ' .. err, 2)
			end

			TableKeysToLowerCase(sprite_info)
            if not sprite_info['image'] then
                error('Failed to load sprite because it has not defined an image resource to use.', 2)
            end
            sprite_cache[self.filename] = sprite_info
            
            -- Look for the image relative to the sprite's containing directory.
            self.image = vx.Image(self.filename:sub(1, self.filename:find('/[^/]*$') - 1) .. '/' .. sprite_info['image'])
            sprite_image_cache[self.filename] = self.image
            
            local flip = vx.Image(self.image)
            
            v3.FlipBlit(0, 0, true, false, self.image.image_handle, flip.image_handle)
            sprite_flipped_image_cache[self.filename] = flip
            self.flipped_image = flip
		else
			sprite_info = sprite_cache[self.filename]
			self.image = sprite_image_cache[self.filename]
			self.flipped_image = sprite_flipped_image_cache[self.filename]
		end
	
		self.animations = sprite_info['animations'] or {}
		for strand, script in pairs(self.animations) do
			self.animations[strand] = script:upper()
		end
		if sprite_info['death_sound'] and resources.sounds[sprite_info['death_sound']] then
			self.death_sound = resources.sounds[sprite_info['death_sound']]
		end
		self.animation = AnimationScript()
        if not tonumber(sprite_info['frame_width']) then
            error('Failed to load sprite because it is does not specify its frame width (or it is not a number)', 2)
        end
        if not tonumber(sprite_info['frame_height']) then
            error('Failed to load sprite because it is does not specify its frame height (or it is not a number)', 2)
        end
        if not tonumber(sprite_info['frame_columns']) then
            error('Failed to load sprite because it is does not specify its number of frame columns (or it is not a number)', 2)
        end
        self.frame_width = tonumber(sprite_info['frame_width'])
        self.frame_height = tonumber(sprite_info['frame_height'])
        self.frame_columns = tonumber(sprite_info['frame_columns'])
        self.frame_padding = tonumber(sprite_info['frame_padding']) or 1
        
        self.frame_buffer = vx.Image(self.frame_width, self.frame_height)
		
		self.hotspots = sprite_info['hotspots'] or { main = { x = 0, y = 0, width = self.frame_width, height = self.frame_height } }
		
		self.z_index = 0
		self.visible = true
		self.dispose = false
        self.tint = false
		self.direction = direction.Right
		self.lucent = 0
        
		
		self.investigate_action = nil
        
		table.insert(sprites, self)
	end
		
	function Sprite:GetAnimation(strand)
		if self.animations[strand:lower()] then
			return self.animations[strand:lower()]
		else
			error("The animation '" .. strand .. "' does not exist for the sprite '" .. self.filename .. "'.", 2)
		end
	end
	
	function Sprite:GetHotspot(h)
		local t = self.hotspots[h:lower()]
		if t then
			if self.direction == direction.Right or h == 'main' then
				return t
			else
				return { x = self.frame_width - t.x, y = t.y, width = t.width, height = t.height }
			end
			
		else
			error("The hotspot '" .. h .. "' does not exist for the sprite '" .. self.filename .. "'.", 2)
		end
	end
	
	function Sprite:SetAnimation(strand)
		-- Different animation.
		if strand ~= self.animation.strand then
			self.animation.strand = strand
			self.animation.script = self:GetAnimation(strand)
			self.animation.wait = 0
			self.animation.offset = 1
		end
	end
	
	function Sprite:Touches(sprite)
		local self_hotspot = self:GetHotspot('main')
		local spr_hotspot = sprite:GetHotspot('main')
		return self.x <= sprite.x + spr_hotspot.width
			and self.x + self_hotspot.width >= sprite.x
			and self.y <= sprite.y + spr_hotspot.height
			and self.y + self_hotspot.height >= sprite.y
	end
	
	function Sprite:Update()
		self.animation:Parse()
	end
	
	local get_xwin = v3.get_xwin
	local get_ywin = v3.get_ywin
	function Sprite:Render()
		if not self.visible then
			return
		end
		
		local x = self.x - get_xwin()
		local y = self.y - get_ywin()
		local f = self.animation.frame

        local hotspot = self:GetHotspot('main')
        local fx = math.floor(f % self.frame_columns) * (self.frame_width + self.frame_padding) + self.frame_padding
        local fy = math.floor(f / self.frame_columns) * (self.frame_height + self.frame_padding) + self.frame_padding
        
        self.frame_buffer:RectFill(0, 0, self.frame_buffer.width, self.frame_buffer.height, vx.RGB(255, 0, 255))
        
        if self.direction == direction.Right then
            self.image:GrabRegion(fx, fy, fx + self.frame_width - 1, fy + self.frame_height - 1, 0, 0, self.frame_buffer)
        else
            local flip = self.flipped_image
            flip:GrabRegion(flip.width - 1 - fx, fy, flip.width - 1 - (fx + self.frame_width - 1), fy + self.frame_height - 1, 0, 0, self.frame_buffer)
        end
        
        if self.tint then
            self.frame_buffer:ColorFilter(0, self.tint)
        end
        
        vx.SetLucent(self.lucent)
        self.frame_buffer:Blit(x - hotspot.x, y - hotspot.y)
        vx.SetLucent(0)
    end
end
