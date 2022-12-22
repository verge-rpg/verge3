-- Load the game font.
font = vx.Font('resources/fonts/thin.png')
font_pink = vx.Font('resources/fonts/thinpink.png')

require 'controls'
require 'frame_limiter'
require 'direction'
require 'sprite'
require 'entity'
require 'player'
require 'projectile'
require 'box_style'
require 'dialogue'
require 'ability'
require 'minimap'
require 'progress'
require 'npc'
require 'ai'
require 'camera'
require 'resources'

-- Load the resources.
resource_loader.Load()

-- The acceleration of gravity
GRAVITY = 0.09
-- The max fall speed
TERMINAL_VELOCITY = 6.0

-- The map to run on startup
TEST_MAP = 'resources/maps/molasses.map'
-- The sprite file to load for the hero
PLAYER_SPRITE = 'resources/sprites/molasses_meow.sprite'
PLAYER_SPRITE2 = 'resources/sprites/molasses_meow2.sprite'

player = nil
map_switched = false
has_map = false
map_spawn_x = -1
map_spawn_y = -1
map_spawn_loc = false

textbox_active = false
textbox_freeze = false
textbox_jumble = false

sound_shot_timestamp = false
sound_slam_timestamp = 0

camera_scroll = false

song_house = vx.Song('resources/songs/ambienttest.ogg')
--song_forest = vx.Song('cd-level5.it')


show_debug = false

sprites = {}

destroyed_tiles = {}
function DestroyTile(x, y, t)
    x = math.floor(x)
    y = math.floor(y)
    
    for i, tile in ipairs(destroyed_tiles) do
        if tile.x == x and tile.y == y then
            return false
        end
    end
    
    local tile = { x = x, y = y, tile = vx.map:GetTile(x, y, 1), time = (t and t.time) or 300, door = (t and t.door) }

    table.insert(destroyed_tiles, tile)
    vx.map:SetTile(x, y, 1, 0)
    vx.map:SetObs(x, y, 0)
    if tile.door then
        tile.tile2 = vx.map:GetTile(x + 1, y, 1)
        vx.map:SetTile(x + 1, y, 1, 0)
        vx.map:SetObs(x + 1, y, 0)
    end
    return true
end

function Startup()
    InitBullets()
    InitAbilities()
    if not LoadGame() then
        MapSwitch(TEST_MAP)
    end
end

teleport_timestamp = 0
function TeleportTo(x, y, location_text)
    if teleport_timestamp > vx.clock.systemtime then
        return
    end
    
    resources.sounds.teleport:Play()
    player.x = x
    player.y = y
    if location_text then
        dialogue.LocationText(location_text)
    end
    camera:SetTarget(player)
    teleport_timestamp = vx.clock.systemtime + 50
end

function MapSwitch(...)
    t = {...}
    local filename
    if type(t[1]) == 'string' then
        map_spawn_x = -1
        map_spawn_y = -1
        filename = t[1]
        map_spawn_loc = false
    else
        map_spawn_x = t[1]
        map_spawn_y = t[2]
        filename = t[3]
        map_spawn_loc = t[4] or false
    end
    
    vx.map:Switch(filename)
    map_switched = true
end

function WarpEvent(event, ...)
    vx.map:SetTile(event.x, event.y, 1, 0)
    
    local ticks = 45
    local x = math.floor(event.x) * 16
    local y = math.floor(event.y) * 16
    
    resources.sounds.rage:Play()
    player.Update = function(self)
        self.x = x
        self.y = y
        self.x_speed = 0
        self.y_speed = 0
        
        if ticks > 0 then
            ticks = ticks - 1
            if ticks == 0 then
                resources.sounds.dark_teleport:Play()
            end
        else
            self.rot = self.rot + 2
            self.scale = self.scale + 2
        end
        
        if self.scale > 320 then
            self.Update = self.DefaultUpdate
            MapSwitch(unpack(arg))
        end
        
        -- Update the entity
        Entity.Update(self)
        -- Make the player flashy.
        player:SetAnimation('flash')
    end
end

function TilesetSwitch(image_filename)
    local tileset = vx.map.tileset
    local tiles = math.floor(tileset.width / 16)
    local image = vx.Image(image_filename)
    local row = math.floor(image.height / 16)
    local col = math.floor(image.width / 16)

    for i = 0, row - 1 do
        for j = 0, col - 1 do
            local x = j * 16
            local y = i * 16
            image:FullGrabRegion(x, y, x + 15, y + 15, 0, (i * col + j) * 16, tileset)
        end
    end
end

render_list = {}
function PrepareMap()
    sprites = {}
    
    render_list = {}
    print('Render string "' .. vx.map.render_string .. '"')
    for item in string.gmatch(vx.map.render_string, "[^,]*,") do
        print('Item ' .. item)
    end
    
    PrepareMinimap()
    
    if map_spawn_x == -1 or map_spawn_y == -1 then
        map_spawn_x = vx.map.start_x
        map_spawn_y = vx.map.start_y
    end
    
    player = Player(map_spawn_x * 16, map_spawn_y * 16, PLAYER_SPRITE)
    
    vx.camera:TargetNothing()
    map_switched = false
    
    v3.HookRetrace('RenderSprites')
    
    if map_spawn_loc then
        dialogue.LocationText(map_spawn_loc)
    end
end

function Render()
    local t = vx.screen
    
    vx:Render()
    if show_debug then
        font:Print(5, 5, tostring(frame_limiter.frame_rate) .. ' FPS, ' .. tostring(#sprites) .. ' sprites')
        font:Print(5, 15, tostring(player.scale) .. 'x Scale')
    end
    minimap:Render()
    roaming_text_box:Render()
    location_box:Render()
    info_box:Render()
    --player:RenderHealthGauge()
end

function RenderSprites()
    table.sort(sprites, function(ls, rs) return ls.z_index < rs.z_index end)
    for idx, s in ipairs(sprites) do
        s:Render()
    end
end

ShowPage = vx.ShowPage


do 
    local layers = nil
    local smooth = false
    function InitLayers(value)
        layers = value
        smooth = layers.smooth_transition or false
        layers.smooth_transition = nil
    end
    
    function GetLayer(name)
        return layers[name]
    end

    function HandleInteriorLayers()
        local found = nil
        for layer_name, layer_index in pairs(layers) do
            if vx.map:GetTile(math.floor(camera.x / 16), math.floor(camera.y / 16), layer_index) ~= 0 then
                found = layer_index
            end
        end
        if found then
            for layer_name, layer_index in pairs(layers) do
                if layer_index == found then
                    vx.map.layer[layer_index].lucent = 100
                    if camera.can_move_free then
                        camera.free = (layer_name == 'outer_cover')
                        camera.smooth_transition = smooth or camera.free
                    end
                else
                    vx.map.layer[layer_index].lucent = 0
                end
            end
        end
    end
end


function Update()
    if flags[FLAG_ITEM_MAP] then
        if button.MapToggle.pressed then
            button.MapToggle.pressed = false
            has_map = not has_map
        end
    end
    
    location_box:Update()
    info_box:Update()
    roaming_text_box:Update()
    minimap:Update()
    if not textbox_active and not textbox_freeze then
        for idx, s in ipairs(sprites) do
            s:Update()
        end
    elseif not textbox_freeze then
        player:Update()
    end

    camera:Update()
    HandleInteriorLayers()
    
    for idx, tile in ipairs(destroyed_tiles) do
        if tile.time > 0 then
            tile.time = tile.time - 1
        elseif  (not tile.door
                    and (math.abs(math.floor((player.x + 8) / 16) - tile.x) > 1
                        or math.abs(math.floor((player.y + 8) / 16) - tile.y) > 1))
                or (tile.door
                    and (math.abs(math.floor(player.x / 16) - tile.x) > 2
                        or math.abs(math.floor((player.y + 8) / 16) - tile.y) > 8))
                then
                    if tile.door then
                        if sound_slam_timestamp < vx.clock.systemtime then
                            resources.sounds.door_close:Play()
                            sound_slam_timestamp = vx.clock.systemtime + 10
                        end
                        vx.map:SetTile(tile.x + 1, tile.y, 1, tile.tile2)
                        vx.map:SetObs(tile.x + 1, tile.y, 1)
                    end
                    vx.map:SetTile(tile.x, tile.y, 1, tile.tile)
                    vx.map:SetObs(tile.x, tile.y, 1)
                    tile.repaired = true
        else
            tile.time = 50
        end
    end
    
    local hotspot = player:GetHotspot('main')

    
    -- Remove any sprites newly flagged for disposal, while avoiding
    -- the undefined behaviour that occurs if you
    -- you delete while iterating
    do
        local idx = 1
        while idx <= #sprites do
            if sprites[idx].dispose then
                table.remove(sprites, idx)
            else
                idx = idx + 1
            end
        end
    end
    -- Remove any destroyed tiles that have been repaired
    do
        local idx = 1
        while idx <= #destroyed_tiles do
            if destroyed_tiles[idx].repaired then
                table.remove(destroyed_tiles, idx)
            else
                idx = idx + 1
            end
        end
    end
    if map_switched then
        return
    end
end

function ProcessSidescrollingEngine()
    camera:SetTarget(player)
    camera.smooth_transition = true
    while true do       
        Render()
        ShowPage()
        
        vx.UpdateControls()
        frame_limiter:Input()
        frame_limiter:Update()
        
        -- Fixes a problem with full-screen switching, where the
        -- unpress events for the ALT+ENTER keys aren't registered.
        if vx.key.Alt.pressed and vx.key.Enter.pressed then
            vx.key.Alt.pressed = false
            vx.key.Enter.pressed = false
        end
        
        if vx.key.F12.pressed then
            vx.screen:CopyToClipboard()
            vx.key.F12.pressed = false
        end
        if button.Debug.pressed then
            show_debug = not show_debug
            button.Debug.pressed = false
        end
        --[[if vx.key.L.pressed then
            LoadGame()
            vx.key.L.pressed = false
        end]]
        
        --[[if frame_limiter.gap == 0 then
            v3.Sleep(10);
        end]]
        
        for i = 1, frame_limiter.gap do
            Update()
        end
        if map_switched then
            break
        end
    end
end
