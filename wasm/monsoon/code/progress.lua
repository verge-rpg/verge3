FLAG_ITEM_JUMP = 1
FLAG_ITEM_PEASHOOTER = 2
FLAG_ITEM_SOCKS = 3
FLAG_ITEM_OUTDOOR_KEY = 4
FLAG_ITEM_BOTTLE = 5
FLAG_ITEM_DIAMOND = 6

FLAG_PLOT_INTRO = 7
FLAG_PLOT_NEEDOUTDOORKEY = 8

FLAG_TOTAL = 32

flags = {}
for i = 1, FLAG_TOTAL do
    flags[i] = 0
end

items = {
    jump = {
        flag = FLAG_ITEM_JUMP;
        map = 'molasses.map';
        x = 6; y = 9;
        equip = function()
            player.can_jump = true
        end;
    };
    socks = {
        flag = FLAG_ITEM_SOCKS;
        map = 'molasses.map';
        x = 10; y = 79;
        equip = function()
            player.max_jump_height = 96
            player.jump_speed = 2
        end;
    };
    pea_shooter = {
        flag = FLAG_ITEM_PEASHOOTER;
        map = 'molasses.map';
        x = 84; y = 33;
        equip = function()
            player.weapon_ability = abilities['pea_shooter']
            player.max_jump_height = 96
            player.jump_speed = 2
        end;
    };
    outdoor_key = {
        flag = FLAG_ITEM_OUTDOOR_KEY;
        map = 'molasses.map';
        x = 43; y = 80;
    };
    bottle = {
        flag = FLAG_ITEM_BOTTLE;
        map = 'molasses.map';
        x = 71; y = 2;
    };
    diamond = {
        flag = FLAG_ITEM_DIAMOND;
        map = 'molasses.map';
        x = 42; y = 17;
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
        ProcessItem(item)
        flags[item.flag] = 1
    end
end

function ProcessFlags()
    for _, item in pairs(items) do
        if flags[item.flag] > 0 then
            ProcessItem(item)
        end
    end
end