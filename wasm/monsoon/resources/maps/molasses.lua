local layers = {
    inner_cover = 3;
    outer_cover = 4;
};

local spawns = {
    {
        class = NPC;
        capture = 'duck1';
        filename = 'resources/sprites/bird.sprite';
        x = 148 * 16; y = 8 * 16;
        update_callback = AI.TriangleWave;
    };
    {
        class = NPC;
        capture = 'duck2';
        filename = 'resources/sprites/bird.sprite';
        x = 141 * 16; y = 64 * 16;
        update_callback = AI.TriangleWave;
        direction = direction.Left
    };
    {
        class = NPC;
        capture = 'duck3';
        filename = 'resources/sprites/bird.sprite';
        x = 145 * 16; y = 66 * 16;
        update_callback = AI.TriangleWave;
        direction = direction.Right
    };
    {
        class = NPC;
        capture = 'duck4';
        filename = 'resources/sprites/bird.sprite';
        x = 155 * 16; y = 68 * 16;
        update_callback = AI.TriangleWave;
        direction = direction.Left
    };
    {
        class = NPC;
        capture = 'duck5';
        filename = 'resources/sprites/bird.sprite';
        x = 143 * 16; y = 70 * 16;
        update_callback = AI.TriangleWave;
        direction = direction.Right
    };
    {
        class = NPC;
        capture = 'hermit';
        filename = 'resources/sprites/hermit.sprite';
        x = 162 * 16 + 4; y = 41 * 16;
        health = 5;
        update_callback = AI.StandAndStare;
        investigate_action = function()
            TalkToHermit();
        end;
    };
    {
        class = NPC;
        filename = 'resources/sprites/button.sprite';
        health = 255;
        x = 249 * 16; y = 26 * 16;
        hit_callback = function(self)
            AI.FlickSwitch(self)
            ModifySwitchPuzzle(248, 32)
        end
    };
    {
        class = NPC;
        filename = 'resources/sprites/button.sprite';
        health = 255;
        x = 255 * 16; y = 26 * 16;
        hit_callback = function(self)
            AI.FlickSwitch(self)
            ModifySwitchPuzzle(254, 32)
        end
    };
}

function Start()
    -- TEMPORARY STUFF
    --[[flags[FLAG_PLOT_INTRO] = 1
    flags[FLAG_ITEM_JUMP] = 1
    flags[FLAG_ITEM_PEASHOOTER] = 1
    flags[FLAG_ITEM_SOCKS] = 1
    flags[FLAG_ITEM_OUTDOOR_KEY] = 1
    flags[FLAG_ITEM_MAP] = 1
    flags[FLAG_ITEM_EYE] = 1
    flags[FLAG_ITEM_BOTTLE] = 1
    flags[FLAG_PLOT_SWITCH_PUZZLE] = 1]]
    
    --map_spawn_x = 52; map_spawn_y = 85
    --map_spawn_x = 142; map_spawn_y = 4
    --map_spawn_x = 245; map_spawn_y = 22
    --map_spawn_x = 223; map_spawn_y = 80
    -- TEMPORARY STUFF
    
    PrepareMap()
    ProcessItemFlags()
    for i, spawn in ipairs(spawns) do
        local obj = spawn.class(spawn.x, spawn.y, spawn.filename)
        obj.meta = spawn
        obj:ParseMeta()
    end
    ProcessCaptureFlags()
    InitLayers(layers)
    CheckSwitchPuzzle()
    
    if flags[FLAG_PLOT_INTRO] == 0 then
        --dialogue.ConfinedText("You awake from a pleasant rest, to the sounds of a devestating wind.")
        --dialogue.Wait(50)
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
    dialogue.RoamingText("Press the X key to overwhelm your victims with the eccentric power of neon love.")
    dialogue.RoamingText("Only you, Molasses Meow, can harness such power.")
end

function JumpShoes(event)
    GetItem('socks')
    
    dialogue.RoamingText("You pick up the Lucky Socks. You feel the blue stripes soothing your sensitive feet.")
    dialogue.RoamingText("You can jump higher, in comfort, and in style!")
end

function OutdoorKey(event)
    GetItem('outdoor_key')
    
    dialogue.RoamingText("You pick up the Nebulous Key. This outlandish device is capable of unlocking things.")
    dialogue.RoamingText("Have you seen any locked devices? Maybe you should try this key!")
end

function JumpAbility()
    GetItem('jump')
    
    dialogue.RoamingText("You remembered how to jump! Just press Z to leap into the air!")
end

function BottleThing()
    GetItem('bottle')
    
    dialogue.RoamingText("You pick up the Bottle. With the Bottle at your disposal, you can capture stunned creatures!")
end

function NoIdeaWhatThisIs()
    GetItem('map')
    
    dialogue.RoamingText("You pick up the Map. TODO: You can now view this in the menu screen!")
    dialogue.RoamingText("In the meantime: Press Space to toggle the map at any time.")
end

function WanderingEye()
    GetItem('wandering_eye')
    
    dialogue.RoamingText("You pick up the Wandering Eye.")
    dialogue.RoamingText("You see things you never thought were possible before!")
end

function LiteracyItem(event)
    GetItem('literacy')
    
    dialogue.RoamingText("You've found the Book of Literacy! The book brims with the power of the alphabet.")
    dialogue.RoamingText("You can now use the Down Arrow to decipher and read texts.")
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
                dialogue.RoamingText("You unlocked the door with the Nebulous Key!")
                flags[FLAG_PLOT_NEEDOUTDOORKEY] = 2
            end
        elseif flags[FLAG_PLOT_NEEDOUTDOORKEY] == 0 then
            dialogue.RoamingText("The door leads outside... but this side needs a key.")
            
            flags[FLAG_PLOT_NEEDOUTDOORKEY] = 1
        end
    end
end

function OutsideDoorsOnLeft(event)
    if player.direction == direction.Left and player.moving then
        if flags[FLAG_ITEM_OUTDOOR_KEY] == 1 then
            UnlockOutsideDoors(event.x - 2)
        
            if flags[FLAG_PLOT_NEEDOUTDOORKEY] ~= 2 then
                dialogue.RoamingText("You unlocked the door with the Nebulous Key!")
                flags[FLAG_PLOT_NEEDOUTDOORKEY] = 2
            end
        elseif flags[FLAG_PLOT_NEEDOUTDOORKEY] == 0 then
            dialogue.RoamingText("The door leads inside... but this side needs a key.")
            
            flags[FLAG_PLOT_NEEDOUTDOORKEY] = 1
        end
    end
end

function TalkToHermit()
    dialogue.ConfinedText("TODO: Meaningful dialogue")
end

local FLIP_CYCLE = { [36] = 56; [56] = 57; [57] = 36 };
local CRYSTAL_SWAP = { [72] = 69; [73] = 70; [74] = 71; [92] = 89; [93] = 90; [94] = 91; };
function CheckSwitchPuzzle()
    -- Both kittens. Open sesame.
    if (vx.map:GetTile(248, 32, 0) == vx.map:GetTile(254, 32, 0)
        and vx.map:GetTile(248, 32, 0) == 36 )
        or flags[FLAG_PLOT_SWITCH_PUZZLE] ~= 0 then
            -- "Energize" the grey crystals with cyan.
            for i = 200, 259 do
                for j = 15, 104 do
                    local tile = CRYSTAL_SWAP[vx.map:GetTile(i, j, 1)]
                    if tile then
                        vx.map:SetTile(i, j, 1, tile)
                    end
                    local tile = CRYSTAL_SWAP[vx.map:GetTile(i, j, 2)]
                    if tile then
                        vx.map:SetTile(i, j, 2, tile)
                    end
                end
            end
            
            -- Unlock the door to the cyan portal room
            vx.map:SetTile(219, 82, 1, 0)
            vx.map:SetTile(219, 83, 1, 0)
            vx.map:SetTile(219, 84, 1, 0)
            vx.map:SetTile(219, 85, 1, 0)
            vx.map:SetTile(220, 82, 1, 0)
            vx.map:SetTile(220, 83, 1, 0)
            vx.map:SetTile(220, 84, 1, 0)
            vx.map:SetTile(220, 85, 1, 0)
            vx.map:SetObs(219, 82, 0)
            vx.map:SetObs(219, 83, 0)
            vx.map:SetObs(219, 84, 0)
            vx.map:SetObs(219, 85, 0)
            vx.map:SetObs(220, 82, 0)
            vx.map:SetObs(220, 83, 0)
            vx.map:SetObs(220, 84, 0)
            vx.map:SetObs(220, 85, 0)
            
            FillSwitchPuzzlePiece(248, 32, 36)
            FillSwitchPuzzlePiece(254, 32, 36)
            
            if flags[FLAG_PLOT_SWITCH_PUZZLE] == 0 then
                -- Zelda-like "you found a secret" jingle goes here.
                flags[FLAG_PLOT_SWITCH_PUZZLE] = 1
                dialogue.RoamingText("You hear a mysterious device activate.")
            end
    end
end

function FillSwitchPuzzlePiece(x, y, piece)
    for i = x, x + 2 do
        for j = y, y + 1 do
            vx.map:SetTile(i, j, 0, piece)
        end
    end
end

function ModifySwitchPuzzle(x, y)
    if flags[FLAG_PLOT_SWITCH_PUZZLE] ~= 0 then
        return
    end
    local next = FLIP_CYCLE[vx.map:GetTile(x, y, 0)]
    
    FillSwitchPuzzlePiece(x, y, next)
    CheckSwitchPuzzle()
end

function BigBlueWarpEvent(event)
    WarpEvent(event, 'resources/maps/water.map')
end

function SecretWarpEvent(event)
    WarpEvent(event, 'resources/maps/water.map')
end



local function CreateSign(f)
    return function(event)
        LiteracyCheck(f, event)
    end
end

SignIntro = CreateSign(
    function(event, literate)
        dialogue.ConfinedText("\"How do you like your house? We made a few renovations.#")
        dialogue.Wait(30)
        dialogue.ConfinedText("\"...Evil renovations, to the entire Neoniverse.#")
        dialogue.ConfinedText("\"But that's not all. Oh no.#")
        dialogue.ConfinedText("\"We stole your belongings and littered them in all sorts of inconvenient spots across the world.#")
        dialogue.Wait(30)
        dialogue.ConfinedText("\"Please look forward to many future annoyances. Have a good day.#")
    end
)

SignTaunt = CreateSign(
    function(event, literate)
        dialogue.ConfinedText("\"You see that door over there? Well, it's locked on both sides! You can only get inside with a key.#")
        if literate then
            dialogue.Wait(100)
            dialogue.ConfinedText("This home security measure seems unnecessary to you.")
            dialogue.ConfinedText("In fact, you don't quite recall installing such a device yourself...")
        end
    end
)

SignEyesore = CreateSign(
    function(event, literate)
        dialogue.ConfinedText("\"The Neoniverse. It is said that this one of many realms in existence.#")
        dialogue.ConfinedText("\"Outlanders often find themselves mesmerized and blinded by the strange aesthetics of this world.#")
    end
)

SignLovestone = CreateSign(
    function(event, literate)
        dialogue.ConfinedText("\"Love stones are mysterious sedimentary rocks that form out of unrequited love.#")
		dialogue.ConfinedText("\"They're cheap, ubiquitous, and extremely fire-retardant! These materials are commonly used to construct walkways and homes.#")
        dialogue.Wait(30)
        dialogue.ConfinedText("\"...Including this one.#")
    end
)

SignLovestone2 = CreateSign(
    function(event, literate)
        dialogue.ConfinedText("\"Love stones are a very durable heart-shaped material.#")
        dialogue.ConfinedText("\"A mildly eccentric blast can diminish their power, at least temporarily.#")
        dialogue.ConfinedText("\"Only a concentrated beam of pure passion can permanently destroy them.#")
    end
)

SignOminousWarning = CreateSign(
    function(event, literate)
        dialogue.ConfinedText("\"Below lies the entrance to an evil land, filled with curses and terror.#")
        dialogue.ConfinedText("\"Gyor the chrominancer awaits inside. He creates abominations to the eye.#")
        dialogue.Wait(30)
        dialogue.ConfinedText("\"A magical seal forbids passage through this chamber. We are unapologetic for any inconvenience.#")
    end
)

SignOminousWarning2 = CreateSign(
    function(event, literate)
        dialogue.ConfinedText("\"Turn back, adventurer. Only retinal destruction awaits those who open this gate.#")
        dialogue.ConfinedText("\"Beings that would dare defile the Spectrum of Order have no place in this world.#")
    end
)

SignOminousWarning3 = CreateSign(
    function(event, literate)
        dialogue.ConfinedText("\"Unless you have released the seal, passage further is forbidden.#")
        dialogue.ConfinedText("\"For the good of the Neoniverse, leave this wretched place.#")
    end
)

SignDowner = CreateSign(
    function(event, literate)
        if not literate or flags[FLAG_PLOT_DO_NOT_READ_SIGN] == 0 then
            dialogue.ConfinedText("\"I'm just a sign! This text is boring. Please don't read this.#")
        elseif flags[FLAG_PLOT_DO_NOT_READ_SIGN] == 1 then
            dialogue.ConfinedText("\"I'm just a sign! Nothing to see here.#")
        elseif flags[FLAG_PLOT_DO_NOT_READ_SIGN] == 2 then
            dialogue.ConfinedText("\"I'm just a sign! Please, find something else to read.#")
        elseif flags[FLAG_PLOT_DO_NOT_READ_SIGN] == 3 then
            dialogue.ConfinedText("\"I'm just a sign! You're really annoying.#")
            dialogue.ConfinedText("\"I mean, how would you feel if I started reading YOU?#")
        elseif flags[FLAG_PLOT_DO_NOT_READ_SIGN] == 4 then
            dialogue.ConfinedText("\"I'm just a sign! And this is your last warning.#")
        elseif flags[FLAG_PLOT_DO_NOT_READ_SIGN] >= 5 then
            dialogue.ConfinedText("\"Prepare for punishment!#")
            dialogue.Wait(30)
            dialogue.ConfinedText("The words on the sign begin to melt.")
            dialogue.ConfinedText("TODO: Teleport to another dimension (or something mean)")
        end
        
        if literate then
            flags[FLAG_PLOT_DO_NOT_READ_SIGN] = flags[FLAG_PLOT_DO_NOT_READ_SIGN] + 1
        end
    end
)

SignImmortal = CreateSign(
    function(event, literate)
        dialogue.ConfinedText("\"In this world, no living creature may die, so long as the Spectrum of Order remains.#")
        dialogue.ConfinedText("\"The Spectrum flows through all things, immortalizing and sustaining life.#")
    end
)

SignPuzzleA = CreateSign(
    function(event, literate)
        if flags[FLAG_PLOT_SWITCH_PUZZLE] == 0 then
            dialogue.ConfinedText("\"Reveal your cat-like grace.#")
        else
            dialogue.ConfinedText("\"The corridor invites your presence.#")
        end
    end
)

SignPuzzleB = CreateSign(
    function(event, literate)
        dialogue.ConfinedText("\"The symbols on these pedestals are altered by a controller somewhere...#")
    end
)