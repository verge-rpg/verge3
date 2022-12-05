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
require 'resources'

-- Load the game font.
font = vx.Font('resources/fonts/thin.png')
resource_loader.Load()

-- The acceleration of gravity
GRAVITY = 0.09
-- The max fall speed
TERMINAL_VELOCITY = 6.0

-- The map to run on startup
TEST_MAP = 'resources/maps/molasses.map'
-- The sprite file to load for the hero
PLAYER_SPRITE = 'resources/sprites/molasses_meow.sprite'

player = nil
map_switched = false
has_map = false
map_spawn_x = -1
map_spawn_y = -1

sound_shot_timestamp = false
sound_slam_timestamp = 0

--song_house = vx.Song('cd-level1.it')
--song_forest = vx.Song('cd-level5.it')

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
    MapSwitch(TEST_MAP)
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
    teleport_timestamp = vx.clock.systemtime + 50
end

function MapSwitch(...)
    t = {...}
    local filename
    if type(t[1]) == 'string' then
        map_spawn_x = -1
        map_spawn_y = -1
        filename = t[1]
    else
        map_spawn_x = t[1]
        map_spawn_y = t[2]
        filename = t[3]
    end
    
    vx.map:Switch(filename)
    map_switched = true
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
    
    ProcessFlags()
    
    vx.camera:TargetNothing()
    map_switched = false
    
    v3.HookRetrace('RenderSprites')
end

function Render()
    local t = vx.screen
    
    vx:Render()
    font:Print(5, 5, tostring(frame_limiter.frame_rate) .. ' FPS, ' .. tostring(#sprites) .. ' sprites')
    roaming_text_box:Render()
    location_box:Render()
    --player:RenderHealthGauge()
end

function RenderSprites()
    table.sort(sprites, function(ls, rs) return ls.z_index < rs.z_index end)
    for idx, s in ipairs(sprites) do
        s:Render()
    end
end

function ShowPage()
    vx.ShowPage()
end

function ProcessSidescrollingEngine()
    while true do       
        Render()
        ShowPage()
        
        vx.UpdateControls()
        frame_limiter:Input()
        frame_limiter:Update()
        
        if vx.key.F12.pressed then
            vx.screen:CopyToClipboard()
        end
        
        for i = 1, frame_limiter.gap do
            roaming_text_box:Update()
            location_box:Update()
            for idx, s in ipairs(sprites) do
                s:Update()
            end

            vx.camera.x = math.floor((player.x + 8) / vx.screen.width) * vx.screen.width
            vx.camera.y = math.floor((player.y + 8) / vx.screen.height) * vx.screen.height
            
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
                break
            end
        end
        if map_switched then
            break
        end
    end
end
