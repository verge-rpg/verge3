local MINIMAP_X_SCALE = 4
local MINIMAP_Y_SCALE = 3
local MINIMAP_X_CLIP = 16
local MINIMAP_Y_CLIP = 16
local MINIMAP_BLOCK_WIDTH = 320
local MINIMAP_BLOCK_HEIGHT = 240
local MINIMAP_LINE_COLOR = vx.RGB(184, 114, 248)
local MINIMAP_VISITED_COLOR = vx.RGB(80, 22, 233)
local MINIMAP_ACTIVE_COLOR = vx.RGB(139, 78, 241)
local MINIMAP_UNDISCOVERED_COLOR = vx.RGB(61, 3, 124)
local MINIMAP_BACKGROUND_COLOR = vx.RGB(46, 11, 85)

minimaps = {}
minimap = nil

vergeclass 'Minimap' do
	Minimap.morphing = false
	Minimap.morph_timer = 0
	Minimap.distort = 0
	Minimap.lucent = 0
	Minimap.image = false
	
	function Minimap:__init()
		self.regions = {}
		
		local w = math.ceil(vx.map.width * 16 / MINIMAP_BLOCK_WIDTH)
		local h = math.ceil(vx.map.height * 16 / MINIMAP_BLOCK_HEIGHT)

		for x = 1, w do
			self.regions[x] = {}
			for y = 1, h do
				self.regions[x][y] = MinimapRegion(x, y)
			end
		end

		for x = 1, w do
			for y = 1, h do
				if x > 1 and self.regions[x - 1][y].obstructed then
					self.regions[x][y].left_wall = true
				end
				if y > 1 and self.regions[x][y  - 1].obstructed then
					self.regions[x][y].top_wall = true
				end
				if x < w and self.regions[x + 1][y].obstructed then
					self.regions[x][y].right_wall = true
				end
				if y < h and self.regions[x][y  + 1].obstructed then
					self.regions[x][y].bottom_wall = true
				end
			end
		end
	end
	
	function Minimap.ApplyMorphEffect()
		Minimap.morphing = true
		Minimap.morph_timer = vx.clock.systemtime
	end

	function Minimap:Render()
		local w = #self.regions
		local h = #self.regions[1]
		local xofs = vx.screen.width - 5 - (MINIMAP_X_SCALE * MINIMAP_X_CLIP)
		local yofs = 5
		
		-- Morph effect
		if Minimap.morphing then
			Minimap.distort = math.max(40 - ((vx.clock.systemtime - Minimap.morph_timer) / 2.5), 0)
			Minimap.lucent = 100 - ((vx.clock.systemtime - Minimap.morph_timer) / 80 * 100)
			if vx.clock.systemtime - Minimap.morph_timer >= 200 then
				Minimap.lucent = 0
				Minimap.distort = 0
				Minimap.morphing = false
			end
		end

		--vx.screen:RectFill(xofs, yofs, xofs + MINIMAP_X_SCALE * MINIMAP_X_CLIP, yofs + MINIMAP_Y_SCALE * MINIMAP_Y_CLIP, vx.RGB(0, 31, 95))
		--vx.screen:Rect(xofs, yofs, xofs + MINIMAP_X_SCALE * MINIMAP_X_CLIP, yofs + MINIMAP_Y_SCALE * MINIMAP_Y_CLIP, vx.RGB(0, 255, 255))
		Minimap.image:Fill(MINIMAP_BACKGROUND_COLOR)
		Minimap.image:Rect(0, 0, Minimap.image.width - 1, Minimap.image.height - 1, MINIMAP_LINE_COLOR)
		-- vx.screen:SetClip(xofs, yofs, xofs + MINIMAP_X_SCALE * MINIMAP_X_CLIP, yofs + MINIMAP_Y_SCALE * MINIMAP_Y_CLIP)
		
		local get_xwin = v3.get_xwin
		local get_ywin = v3.get_ywin
		for x = 1, w do
			for y = 1, h do
				local r = self.regions[x][y]
				local tx = (x - get_xwin() / MINIMAP_BLOCK_WIDTH - 1) * MINIMAP_X_SCALE + (MINIMAP_X_SCALE * MINIMAP_X_CLIP / 2) --+ xofs
				local ty = (y - get_ywin() / MINIMAP_BLOCK_HEIGHT - 1) * MINIMAP_Y_SCALE + (MINIMAP_Y_SCALE * MINIMAP_Y_CLIP / 2) --+ yofs
				if not r.obstructed then
					if has_map then
						Minimap.image:RectFill(tx, ty, tx + MINIMAP_X_SCALE, ty + MINIMAP_Y_SCALE, MINIMAP_UNDISCOVERED_COLOR)
					end
					if r.visited then
						Minimap.image:RectFill(tx, ty, tx + MINIMAP_X_SCALE, ty + MINIMAP_Y_SCALE, MINIMAP_VISITED_COLOR)	
					end
					if player.x >= (x - 1) * MINIMAP_BLOCK_WIDTH
						and player.x < x * MINIMAP_BLOCK_WIDTH
						and player.y >= (y - 1) * MINIMAP_BLOCK_HEIGHT
						and player.y < y * MINIMAP_BLOCK_HEIGHT then
						r.visited = true
						Minimap.image:RectFill(tx, ty, tx + MINIMAP_X_SCALE, ty + MINIMAP_Y_SCALE, MINIMAP_ACTIVE_COLOR)
					end
					if r.visited or has_map then
						if r.left_wall then
							Minimap.image:Line(tx, ty, tx, ty + MINIMAP_Y_SCALE, MINIMAP_LINE_COLOR)
						end
						if r.right_wall then
							Minimap.image:Line(tx + MINIMAP_X_SCALE, ty, tx + MINIMAP_X_SCALE, ty + MINIMAP_Y_SCALE, MINIMAP_LINE_COLOR)
						end
						if r.top_wall then
							Minimap.image:Line(tx, ty, tx + MINIMAP_X_SCALE, ty, MINIMAP_LINE_COLOR)
						end
						if r.bottom_wall then
							Minimap.image:Line(tx, ty + MINIMAP_Y_SCALE, tx + MINIMAP_X_SCALE, ty + MINIMAP_Y_SCALE, MINIMAP_LINE_COLOR)
						end
					end
				end
			end
		end

		if Minimap.distort == 0 then
			vx.SetLucent(Minimap.lucent)
			Minimap.image:Blit(xofs, yofs)
			vx.SetLucent(0)
		else
			DistortBlit(xofs, yofs, Minimap.distort, Minimap.lucent, Minimap.image, vx.screen)
		end
		-- vx.screen:SetClip(0, 0, vx.screen.width, vx.screen.height)
		
	end
end

function PrepareMinimap()
	if not Minimap.image then
		Minimap.image = vx.Image(MINIMAP_X_SCALE * MINIMAP_X_CLIP, MINIMAP_Y_SCALE * MINIMAP_Y_CLIP)
	end
	Minimap.morphing = false
	Minimap.morph_timer = 0
	Minimap.distort = 0
	Minimap.lucent = 0

	if not minimaps[vx.map.filename] then
		minimaps[vx.map.filename] = Minimap()
	end
	minimap = minimaps[vx.map.filename]
end

vergeclass 'MinimapRegion' do
	function MinimapRegion:__init(x, y)
		local x1 = (x - 1) * MINIMAP_BLOCK_WIDTH
		local y1 = (y - 1) * MINIMAP_BLOCK_HEIGHT
		local x2 = math.min(x1 + MINIMAP_BLOCK_WIDTH, vx.map.width * 16)
		local y2 = math.min(y1 + MINIMAP_BLOCK_HEIGHT, vx.map.height * 16)
		
		x1 = x1 / 16
		y1 = y1 / 16
		x2 = x2 / 16 - 1
		y2 = y2 / 16 - 1
		
		self.left_wall = false
		self.right_wall = false
		self.top_wall = false
		self.bottom_wall = false
		self.obstructed = false
		
		local left_count = 0
		local right_count = 0
		local top_count = 0
		local bottom_count = 0
		local inner_count = 0
		for x = x1, x2 do
			for y = y1, y2 do
				if vx.map:GetObs(x, y) ~= 0 then
					if x == x1 then
						left_count = left_count + 1
					end
					if x == x2 then
						right_count = right_count + 1
					end
					if y == y1 then
						top_count = top_count + 1
					end
					if y == y2 then
						bottom_count = bottom_count + 1
					end
					if x > x1 + 1 and x < x2 -1 and y > y1 + 1 and y < y2 - 1 then
						inner_count = inner_count + 1
					end
				end
			end
		end
		
		local w = x2 - x1 + 1
		local h = y2 - y1 + 1
		
		if left_count >= h or x1 <= 0 then
			self.left_wall = true
		end
		if right_count >= h or x2 >= vx.map.width - 1 then
			self.right_wall = true
		end		
		if top_count >= w or y1 <= 0 then
			self.top_wall = true
		end
		if bottom_count >= w or y2 >= vx.map.height - 1 then
			self.bottom_wall = true
		end
		if inner_count >= (w - 4) * (h  - 4) then
			self.obstructed = true
		end
		self.visited = false
	end
end
