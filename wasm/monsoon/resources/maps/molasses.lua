--[[

Notes to self:

Clean up NPC spawn code, and come up with a coherent way to keep from overprocessing far off-screen things.

]]
local spawns = {
    {
        class = NPC;
        filename = 'resources/sprites/bird.sprite';
        x = 148 * 16; y = 8 * 16;
        update_callback = ai.TriangleWave;
    };
    {
        class = NPC;
        filename = 'resources/sprites/bird.sprite';
        x = 141 * 16; y = 64 * 16;
        update_callback = ai.TriangleWave;
        direction = direction.Left
    };
    {
        class = NPC;
        filename = 'resources/sprites/bird.sprite';
        x = 145 * 16; y = 66 * 16;
        update_callback = ai.TriangleWave;
        direction = direction.Right
    };
    {
        class = NPC;
        filename = 'resources/sprites/bird.sprite';
        x = 155 * 16; y = 68 * 16;
        update_callback = ai.TriangleWave;
        direction = direction.Left
    };
    {
        class = NPC;
        filename = 'resources/sprites/bird.sprite';
        x = 143 * 16; y = 70 * 16;
        update_callback = ai.TriangleWave;
        direction = direction.Right
    };
    {
        class = NPC;
        filename = 'resources/sprites/hermit.sprite';
        x = 162 * 16 + 4; y = 41 * 16;
        health = 5;
        update_callback = ai.StandAndStare;
        investigate_action = function()
            TalkToHermit();
        end;
    }
}

function Start()    
    -- TEMPORARY STUFF
    flags[FLAG_PLOT_INTRO] = 1
    flags[FLAG_ITEM_JUMP] = 1
    flags[FLAG_ITEM_PEASHOOTER] = 1
    flags[FLAG_ITEM_SOCKS] = 0
    flags[FLAG_ITEM_OUTDOOR_KEY] = 1
    
    map_spawn_x = 142; map_spawn_y = 4
    -- TEMPORARY STUFF
    
    PrepareMap()
    for i, spawn in ipairs(spawns) do
        local obj = spawn.class(spawn.x, spawn.y, spawn.filename)
        obj.meta = spawn
        obj:ParseMeta()
    end
    if flags[FLAG_PLOT_INTRO] == 0 then
        dialogue.ConfinedText("You awake from a pleasant rest, to the sounds of a devestating wind.")
        dialogue.Wait(50)
        dialogue.ConfinedText("Your name is Molasses Meow and you will stop at nothing to figure out what happened.")
        
        dialogue.LocationText("Your Room|song_house")
        flags[FLAG_PLOT_INTRO] = 1
    end
    ProcessSidescrollingEngine()
end

function TeleporterA1()
    TeleportTo(9 * 16 + 8, 21 * 16 + 8, "Mysterious Closet Tunnels")
end

function TeleporterA2()
    TeleportTo(89 * 16 + 8, 4 * 16 + 8, "Hallway")
end

function TeleporterB1()
    TeleportTo(96 * 16 + 8, 24 * 16 + 8)
end

function TeleporterB2()
    TeleportTo(2 * 16 + 8, 56 * 16 + 8)
end

function TeleporterC1()
    TeleportTo(15 * 16 + 8, 34 * 16 + 8)
end

function TeleporterC2()
    TeleportTo(2 * 16 + 8, 77 * 16 + 8)
end

function NeonBuster(event)
    GetItem('pea_shooter')
    
    dialogue.RoamingText("You pick up the Love Buster.\nYou feel a surge of neon energy!")
    dialogue.RoamingText("Press the Z key to overwhelm your victims with the eccentric power of neon love.")
    dialogue.RoamingText("Only you, Molasses Meow, can harness such power.")
end

function JumpShoes(event)
    GetItem('socks')
    
    dialogue.RoamingText("You pick up the Lucky Socks. You feel the blue stripes soothing your sensitive feet.")
    dialogue.RoamingText("You can jump higher, in comfort, and in style!")
end

function OutdoorKey(event)
    GetItem('outdoor_key')
    
    dialogue.RoamingText("You pick up the Outdoor Key. You can now go outside, if you remember where that is!")
end

function JumpAbility()
    GetItem('jump')
    
    dialogue.RoamingText("You remembered how to jump! Just press Space to leap into the air!")
end

function BottleThing()
    GetItem('bottle')
    
    dialogue.RoamingText("A bottle of some sort!")
end

function NoIdeaWhatThisIs()
    GetItem('diamond')
    
    dialogue.RoamingText("What. I don't even.")
    dialogue.RoamingText("To be honest, looks pretty useless. Good work collecting it though!")
end

function UnlockOutsideDoors(x)
    -- Using destroy tile means the doors will eventually shut again.
    local t = { time = 0, door = true }
    
    local opened = DestroyTile(x, 3, t) and DestroyTile(x, 4, t) and DestroyTile(x, 5, t) and DestroyTile(x, 6, t)
    if opened then
        resources.sounds.door_open:Play()
    end
end

function OutsideDoorsOnRight(event)
    if player.direction == direction.Right and player.moving then
        if flags[FLAG_ITEM_OUTDOOR_KEY] == 1 then
            UnlockOutsideDoors(event.x + 1)
        
            if flags[FLAG_PLOT_NEEDOUTDOORKEY] ~= 2 then
                dialogue.RoamingText("You unlocked the door with the Outdoor Key!")
                flags[FLAG_PLOT_NEEDOUTDOORKEY] = 2
            end
        elseif flags[FLAG_PLOT_NEEDOUTDOORKEY] == 0 then
            dialogue.RoamingText("The blue door leads outside... but this side of the door needs a key.")
            dialogue.RoamingText("Your locksmith was probably drunk that day.")
            
            flags[FLAG_PLOT_NEEDOUTDOORKEY] = 1
        end
    end
end

function OutsideDoorsOnLeft(event)
    if player.direction == direction.Left and player.moving then
        UnlockOutsideDoors(event.x - 2)
    
        if flags[FLAG_PLOT_NEEDOUTDOORKEY] ~= 2 then
            dialogue.RoamingText("You opened the door effortlessly.")
            flags[FLAG_ITEM_OUTDOOR_KEY] = 1
            flags[FLAG_PLOT_NEEDOUTDOORKEY] = 2
        end    
    end
end

function TalkToHermit()
    dialogue.ConfinedText("Hello!")
end

