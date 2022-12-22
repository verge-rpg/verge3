local VISIT_UNMARKED = 0
local VISIT_NORMAL = 1
local VISIT_SECRET = 2

local MINIMAP_TILES_WIDE = math.floor(vx.screen.width / 16)
local MINIMAP_TILES_HIGH = math.floor(vx.screen.height / 16)

minimaps = {}
minimap = nil

vergeclass 'Minimap' do
	Minimap.morphing = false
	Minimap.morph_timer = 0
	Minimap.distort = 0
	Minimap.lucent = 0
	Minimap.image = false
	
	function Minimap:__init()
        self.width = nil
        self.height = nil
        self.active_x = nil
        self.active_y = nil
        self.visited = nil
        self.regions = nil
        
        self.calculated = false
    end
		
    function Minimap:Calculate(render_order)
        self.active_x = nil
        self.active_y = nil
        if self.calculated then
            return 
        end
        
		local w = math.ceil(vx.map.width / MINIMAP_TILES_WIDE)
		local h = math.ceil(vx.map.height / MINIMAP_TILES_HIGH)
        if (self.width and self.width ~= w) or (self.height and self.height ~= h) then
            self.visited = nil
        end
        self.width = w
        self.height = h
        
        if not self.visited then
            self.visited = {}
            for i = 1, w * h do
                self.visited[i] = VISIT_UNMARKED
            end
        end
        
        self.regions = {}
        for y = 1, h do
            for x = 1, w do
				self.regions[(y - 1) * w + x] = MinimapRegion(render_order, x, y)
			end
		end
	end
	
	function Minimap.ApplyMorphEffect()
		Minimap.morphing = true
		Minimap.morph_timer = vx.clock.systemtime
	end
    
    function Minimap:GetSecretString(r)
        local s = ''
        local regions = self.regions
        s = s .. (regions[r].left_secret and 'L' or '')
        s = s .. (regions[r].right_secret and 'R' or '')
        s = s .. (regions[r].up_secret and 'U' or '')
        s = s .. (regions[r].down_secret and 'D' or '')        
        return s
    end

    function Minimap:Update()
        local regions = self.regions
        local visited = self.visited
        local x = math.floor(math.floor((player.x + 8) / 16) / MINIMAP_TILES_WIDE) + 1
        local y = math.floor(math.floor((player.y + 8) / 16) / MINIMAP_TILES_HIGH) + 1
        local i = (y - 1) * self.width + x
        --print(x, y, i)
        
        if self.active_x and (self.active_x ~= x or self.active_y ~= y) then
            local p = (self.active_y - 1) * self.width + self.active_x
            print(
                '(' .. tostring(self.active_x) .. ', ' .. tostring(self.active_y) .. ')' .. self:GetSecretString(p)
                .. ' -> (' .. tostring(x) .. ', ' .. tostring(y) .. ')' .. self:GetSecretString(i)
            )
            if self.active_x < x then
                if regions[p].right_secret then
                    visited[p] = VISIT_SECRET
                end
                if regions[i].left_secret then
                    visited[i] = VISIT_SECRET
                end
            end
            if self.active_x > x then
                if regions[p].left_secret then
                    visited[p] = VISIT_SECRET
                end
                if regions[i].right_secret then
                    visited[i] = VISIT_SECRET
                end
            end
            if self.active_y < y then
                if regions[p].down_secret then
                    visited[p] = VISIT_SECRET
                end
                if regions[i].up_secret then
                    visited[i] = VISIT_SECRET
                end
            end
            if self.active_y > y then
                if regions[p].up_secret then
                    visited[p] = VISIT_SECRET
                end
                if regions[i].down_secret then
                    visited[i] = VISIT_SECRET
                end
            end
        end
        
        if visited[i] == VISIT_UNMARKED then
            visited[i] = VISIT_NORMAL
        end
        self.active_x = x 
        self.active_y = y
    end

	function Minimap:Render()
        if not has_map then
            return
        end
        
        local visited = self.visited
        local regions = self.regions
        
        vx.SetLucent(25)
        for y = 1, self.height do
            for x = 1, self.width do
                local i = (y - 1) * self.width + x
                if visited[i] == VISIT_NORMAL then
                    if self.active_x == x and self.active_y == y then
                        regions[i].active_image:Blit(x * MINIMAP_TILES_WIDE, y * MINIMAP_TILES_HIGH)
                    else
                        regions[i].image:Blit(x * MINIMAP_TILES_WIDE, y * MINIMAP_TILES_HIGH)
                    end
                elseif visited[i] == VISIT_SECRET then
                    if self.active_x == x and self.active_y == y then
                        regions[i].active_secret_image:Blit(x * MINIMAP_TILES_WIDE, y * MINIMAP_TILES_HIGH)
                    else
                        regions[i].secret_image:Blit(x * MINIMAP_TILES_WIDE, y * MINIMAP_TILES_HIGH)
                    end
                end
            end
        end
        vx.SetLucent(0)
        --Minimap.image:Blit(xofs, yofs)
	end
end

local function GetRenderOrder()
    local t = {}
    for layer in string.gmatch(vx.map.render_string, '([^,]+)(,?)') do
        table.insert(t, layer)
    end
    return t
end

function PrepareMinimap()
	if not Minimap.image then
		--Minimap.image = vx.Image(MINIMAP_X_SCALE * MINIMAP_X_CLIP, MINIMAP_Y_SCALE * MINIMAP_Y_CLIP)
	end
	Minimap.morphing = false
	Minimap.morph_timer = 0
	Minimap.distort = 0
	Minimap.lucent = 0

    local name = vx.map.filename
	if not minimaps[name] then
		minimaps[name] = Minimap()
	end
	minimap = minimaps[name]
    minimap:Calculate(GetRenderOrder())
end

vergeclass 'MinimapRegion' do
    local WALL_COLOR = vx.RGB(0xFF, 0x84, 0xC6)
    local SECRET_COLOR = vx.RGB(0xFF, 0, 0) --vx.RGB(0xFF, 0xDE, 0x85)
    local BG_COLOR = vx.RGB(0x84, 0x39, 0x9c)
    local ACTIVE_WALL_COLOR = vx.RGB(0x00, 0xFF, 0xFF)
    local ACTIVE_BG_COLOR = vx.RGB(0x00, 0x42, 0x52)

    local function IsObscured(render_order, x, y)
        -- Find layer index of player (cache after first calculation)
        local start = render_order.retrace_layer
        if not start then
            for i = 1, #render_order do
                if render_order[i]:lower() == 'r' then
                    render_order.retrace_layer = i
                    break
                end
            end
            start = render_order.retrace_layer
        end

        -- Scan layers above player, return true if any non-zero tile is found.
        local num = 0
        for i = start + 1, #render_order do
            layer = tonumber(render_order[i])
            if layer
                and (vx.map:GetTile(x, y, layer - 1) == 143
                or vx.map:GetTile(x, y, layer - 1) == 162
                or vx.map:GetTile(x, y, layer - 1) == 163)
            then
                return true
            end
        end
        return false
    end
    
	function MinimapRegion:__init(render_order, x, y)
		local x1 = (x - 1) * MINIMAP_TILES_WIDE
		local y1 = (y - 1) * MINIMAP_TILES_HIGH
		local x2 = x1 + (MINIMAP_TILES_WIDE - 1)
		local y2 = y1 + (MINIMAP_TILES_HIGH - 1)
        
        self.secret_image = vx.Image(MINIMAP_TILES_WIDE, MINIMAP_TILES_HIGH)        
        vx.SetLucent(0)
        self.secret_image:Fill(BG_COLOR)
        
        
        has_secret = false
        
        self.left_secret = false
        self.right_secret = false
        self.up_secret = false
        self.down_secret = false
        
        for y = y1, y2 do
            -- Check for secret on the left exit.
            if x1 > 0 and vx.map:GetObs(x1, y) == 0 and vx.map:GetObs(x1 - 1, y) == 0 and IsObscured(render_order, x1, y) then
                has_secret = true
                self.left_secret = true
            end
            -- Check for secret on the right exit.
            if x2 < vx.map.width and vx.map:GetObs(x2, y) == 0 and vx.map:GetObs(x2 + 1, y) == 0 and IsObscured(render_order, x2, y) then
                has_secret = true
                self.right_secret = true
            end
        end
        
        for x = x1, x2 do
            -- Check for secret on the top exit.
            if y1 > 0 and vx.map:GetObs(x, y1) == 0 and vx.map:GetObs(x, y1 - 1) == 0 and IsObscured(render_order, x, y1) then
                has_secret = true
                self.up_secret = true
            end
            -- Check for secret on the bottom exit.
            if y2 < vx.map.height and vx.map:GetObs(x, y2) == 0 and vx.map:GetObs(x, y2 + 1) == 0 and IsObscured(render_order, x, y2) then
                has_secret = true
                self.down_secret = true
            end
        end
        
        local image = self.secret_image
        for y = y1, y2 do
            for x = x1, x2 do
                -- Fill in drawing.
                if vx.map:GetObs(x, y) > 0 then
                    image:SetPixel(x - x1, y - y1, WALL_COLOR)
                elseif has_secret and IsObscured(render_order, x, y) then
                    image:SetPixel(x - x1, y - y1, SECRET_COLOR)
                end
            end
        end
        
        self.image = vx.Image(self.secret_image)
        self.image:ColorReplace(SECRET_COLOR, WALL_COLOR)
        
        self.secret_image:ColorReplace(SECRET_COLOR, BG_COLOR)
        
        self.active_secret_image = vx.Image(self.secret_image)
        self.active_secret_image:ColorReplace(WALL_COLOR, ACTIVE_WALL_COLOR)
        self.active_secret_image:ColorReplace(BG_COLOR, ACTIVE_BG_COLOR)
        
        self.active_image = vx.Image(self.image)
        self.active_image:ColorReplace(WALL_COLOR, ACTIVE_WALL_COLOR)
        self.active_image:ColorReplace(BG_COLOR, ACTIVE_BG_COLOR)
	end
end
