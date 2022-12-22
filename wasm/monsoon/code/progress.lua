FLAG_ITEM_JUMP = 1
FLAG_ITEM_PEASHOOTER = 2
FLAG_ITEM_SOCKS = 3
FLAG_ITEM_OUTDOOR_KEY = 4
FLAG_ITEM_BOTTLE = 5
FLAG_ITEM_MAP = 6
FLAG_ITEM_EYE = 7
FLAG_ITEM_LITERACY = 8

FLAG_PLOT_INTRO = 20
FLAG_PLOT_NEEDOUTDOORKEY = 21
FLAG_PLOT_SWITCH_PUZZLE = 22
FLAG_PLOT_DO_NOT_READ_SIGN = 23

FLAG_TOTAL = 100

flags = {}
for i = 1, FLAG_TOTAL do
    flags[i] = 0
end

items = {
    jump = {
        name = 'Jump';
        flag = FLAG_ITEM_JUMP;
        map = 'molasses.map';
        x = 6; y = 9;
        equip = function()
            player.can_jump = true
        end;
    };
    socks = {
        name = 'Lucky Socks';
        flag = FLAG_ITEM_SOCKS;
        map = 'molasses.map';
        x = 10; y = 79;
        equip = function()
            player.max_jump_height = 80
            player.start_jump_speed = 2
            player.end_jump_speed = 0.8
        end;
    };
    pea_shooter = {
        name = 'Love Buster';
        flag = FLAG_ITEM_PEASHOOTER;
        map = 'molasses.map';
        x = 84; y = 33;
        equip = function()
            player.weapon_ability = abilities['pea_shooter']
        end;
    };
    outdoor_key = {
        name = 'Nebulous Key';
        flag = FLAG_ITEM_OUTDOOR_KEY;
        map = 'molasses.map';
        x = 43; y = 80;
    };
    bottle = {
        name = 'Bottle';
        flag = FLAG_ITEM_BOTTLE;
        map = 'molasses.map';
        x = 71; y = 2;
    };
    map = {
        name = 'Map';
        flag = FLAG_ITEM_MAP;
        map = 'molasses.map';
        x = 42; y = 17;
        equip = function()
            has_map = true
            minimap:ApplyMorphEffect()
        end
    };
    wandering_eye = {
        name = 'Wandering Eye';
        flag = FLAG_ITEM_EYE;
        map = 'molasses.map';
        x = 126; y = 79;
        equip = function()
            -- TODO.
            --[[camera.can_move_free = true
            camera.smooth_transition = true
            SHOW_PARTICLES = true
            
            TilesetSwitch("resources/maps/molasses_tileset3.png")
            local spr = Sprite(0, 0, PLAYER_SPRITE2)
            player.image = spr.image
            player.flipped_image = spr.flipped_image
            player.animations = spr.animations
            table.remove(sprites)]]
        end;
    };
    literacy = {
        name = 'Book of Literacy';
        flag = FLAG_ITEM_LITERACY;
        map = 'molasses.map';
        x = 49; y = 13;
    };
}

function ProcessItem(item)
    if vx.map.filename == 'resources/maps/' .. item.map then
        vx.map:SetTile(item.x, item.y, 1, 0)
        vx.map:SetObs(item.x, item.y, 0)
        vx.map:SetZone(item.x, item.y, 0)
    end
    if item.equip then
        item.equip()
    end
end

function GetItem(item_name)
    local item = items[item_name]
    if flags[item.flag] == 0 then
        resources.sounds.pickup:Play()
        ProcessItem(item)
        dialogue.InfoText(item.name)
        flags[item.flag] = 1
    end
end

function ProcessItemFlags()
    for _, item in pairs(items) do
        if flags[item.flag] > 0 then
            ProcessItem(item)
        end
    end
end

CAPTURE_TOTAL = 64
capture_info = {}
capture_flags = {}
for i = 1, CAPTURE_TOTAL do
    capture_flags[i] = 0
end

local capture_info_index = 0
function CaptureEntry(t)
    capture_info_index = capture_info_index + 1
    
    capture_info[capture_info_index] = t
    capture_flags[capture_info_index] = 0
    
    return capture_info_index
end

capture_entries = {
    duck1 = CaptureEntry {
        name = 'Dimensional Duck';
        desc = 'A mysterious duck, and frequent traveller to other worlds.';
    };
    duck2 = CaptureEntry {
        name = 'Dreamy Duck';
        desc = 'A majestic duck that is the envy of all the others. Her secret is in the shampoo.';
    };
    duck3 = CaptureEntry {
        name = 'Dynamic Duck';
        desc = 'A spirited and energetic duck. She likes flying places.';
    };
    duck4 = CaptureEntry {
        name = 'Diligent Duck';
        desc = 'A quiet and concentrated duck. She bakes apple pies on her weekends off.';
    };
    duck5 = CaptureEntry {
        name = 'Dizzy Duck';
        desc = 'Spinning back and forth, this poor duck has lost her sense of direction.';
    };
    hermit = CaptureEntry {
        name = 'Henry Hermit';
        desc = 'A bearded geezer who dislikes being bothered. He has family issues, but his hat hides it well.';
    };
}

function GetCapture(name)
    local i = capture_entries[name]
    
    capture_flags[i] = 1
    dialogue.InfoText(capture_info[i].name)
end

function ProcessCaptureFlags()
    for _, spr in ipairs(sprites) do
        if spr.meta and spr.meta.capture then
            local i = capture_entries[spr.meta.capture]
        
            if capture_flags[i] > 0 then
                spr.visible = false
            end
        end
    end
end

function InvestigateCheck(f, event)
    if button.Down.pressed then
        button.Down.pressed = false
        f(event)
    end
end

function LiteracyCheck(f, event)
    if button.Down.pressed then
        button.Down.pressed = false
        
        resources.sounds.investigate:Play()
        
        textbox_jumble = flags[FLAG_ITEM_LITERACY] == 0
        if flags[FLAG_ITEM_LITERACY] == 0 then
            textbox_jumble = true
        end
        f(event, flags[FLAG_ITEM_LITERACY] > 0)
        
        textbox_jumble = false
        if flags[FLAG_ITEM_LITERACY] == 0 then
            dialogue.Wait(70)
            dialogue.ConfinedText("...Huh? None of these words make any sense! Perhaps you should learn how to read.")
        end
        --end
    end
end

local HEADER_TEXT = 'molasses-monsoon'
function LoadGame()
    local VERSION = 0
    local f = vx.File('save.dat', vx.FileMode.Read)
    if not f.opened then
        return false
    end
    
    if f:ReadLine() ~= HEADER_TEXT then
        vx.Exit('Invalid save file!')
    end
    
    local v = f:ReadQuad()
    if v ~= VERSION then
        vx.Exit('This version of the game does not support version ' .. tostring(v) .. ' saves!')
    end
    
    local spawn_x = f:ReadQuad()
    local spawn_y = f:ReadQuad()
    local spawn_map = f:ReadString()
    local spawn_loc = f:ReadString()
    local spawn_sng = f:ReadString()
    
    for i = 1, FLAG_TOTAL do
        flags[i] = f:ReadQuad(flags[i])
    end
    for i = 1, CAPTURE_TOTAL do
        capture_flags[i] = f:ReadQuad(flags[i])
    end
    
    local minimap_count = f:ReadQuad()
    -- Write an unordered set of minimaps, one after another.
    minimaps = {}
    for i = 1, minimap_count do
        local k = f:ReadString()
        local mm = Minimap()
        minimaps[k] = mm
        mm.width = f:ReadByte()
        mm.height = f:ReadByte()
        mm.visited = {}
        for i = 1, mm.width * mm.height do
            mm.visited[i] = f:ReadByte()
        end
    end
    
    f:Close()
    
    MapSwitch(spawn_x, spawn_y, spawn_map, spawn_loc .. '|' .. spawn_sng)
    return true
end


function SaveGame()
    local VERSION = 0
    local f = vx.File('save.dat', vx.FileMode.Write)
    
    f:WriteLine(HEADER_TEXT)
    f:WriteQuad(VERSION)
    
    f:WriteQuad(player.x / 16)
    f:WriteQuad(player.y / 16)
    f:WriteString(vx.map.filename)
    f:WriteString(current_location_text)
    f:WriteString(current_song_name)
    
    for i = 1, FLAG_TOTAL do
        f:WriteQuad(flags[i])
    end
    for i = 1, CAPTURE_TOTAL do
        f:WriteQuad(capture_flags[i])
    end
    
    local minimap_count = 0
    for k, mm in pairs(minimaps) do
        minimap_count = minimap_count + 1
    end
    -- Write how many minimaps to expect
    f:WriteQuad(minimap_count)
    -- Write an unordered set of minimaps, one after another.
    for k, mm in pairs(minimaps) do
        f:WriteString(k)
        f:WriteByte(mm.width)
        f:WriteByte(mm.height)
        for i = 1, mm.width * mm.height do
            f:WriteByte(mm.visited[i])
        end
    end
    
    f:Close()
    player:ResetInvincibility()
end

function SaveBlock(x, y)
    SaveGame()
    resources.sounds.save:Play()
    dialogue.InfoText('Saved!')
end

