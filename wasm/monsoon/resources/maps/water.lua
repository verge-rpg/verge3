local layers = {
    inner_cover = 3;
    outer_cover = 4;
    smooth_transition = true;
};

local spawns = {
}
function Start()
    PrepareMap()
    ProcessItemFlags()
    for i, spawn in ipairs(spawns) do
        local obj = spawn.class(spawn.x, spawn.y, spawn.filename)
        obj.meta = spawn
        obj:ParseMeta()
    end
    ProcessCaptureFlags()
    InitLayers(layers)
    
    dialogue.LocationText("Indigo Falls|song_falls")
    ProcessSidescrollingEngine()
end