
resource_loader = {}
resources = nil

local WINDOW_COLOR_FILL = vx.RGB(0xE, 0xE, 0x12)
local WINDOW_COLOR_FILL2 = vx.RGB(0x84, 0x38, 0x9C)
local WINDOW_COLOR_OUTLINE = vx.RGB(0xFB, 0xD9, 0xB3)

function resource_loader.Load()
    resources = {}
    resource_loader.LoadFolder("images", {"png", "gif"}, vx.Image)
    resource_loader.LoadFolder("sounds", {"wav", "ogg"}, vx.Sound)
    resource_loader.LoadFolder("songs", {"mod", "s3m", "xm", "it", "ogg", "mp3"}, vx.Song)
    resource_loader.DrawProgressGauge("Resources loaded.", 1, 1)
end

function resource_loader.LoadFolder(folder, ext, resource_class)
    local i = 0
    local token_list = ""
    for i, val in ipairs(ext) do
        token_list = token_list .. v3.ListFilePattern("resources/" .. folder .. "/*." .. val)
    end
    local count = v3.TokenCount(token_list, "|")
    
    resources[folder] = {}
    while i < count do
        local s = v3.GetToken(token_list, "|", i)
        resource_loader.DrawProgressGauge("Loading '" .. s .. "'...", i, count)
        
        if s ~= "." and s ~= ".." then
            -- Do reference voodoo to resolve to something like
            -- resources.images.myimage = vx.Image("myimage.png")
            resources[folder][s:sub(1, s:find("%.[^.]*$") - 1)] = resource_class("resources/" .. folder .. "/" .. s)

            print(s)
        end
        i = i + 1
    end
    resource_loader.DrawProgressGauge("Loaded all " .. folder .. ".", 1, 1)
end

function resource_loader.DrawProgressGauge(message, current, total)
    local progress = current / total * 100
    vx.screen:RectFill(0, 0, vx.screen.width, vx.screen.height, WINDOW_COLOR_FILL)
    vx.screen:RectFill(vx.screen.width / 2 - 50, vx.screen.height / 2 - 15, vx.screen.width / 2 - 50 + progress, vx.screen.height / 2 - 10, WINDOW_COLOR_FILL2)
    vx.screen:Rect(vx.screen.width / 2 - 50, vx.screen.height / 2 - 15, vx.screen.width / 2 - 50 + progress, vx.screen.height / 2 - 10, WINDOW_COLOR_OUTLINE)
    vx.screen:Rect(vx.screen.width / 2 - 50, vx.screen.height / 2 - 15, vx.screen.width / 2 + 50, vx.screen.height / 2 - 10, WINDOW_COLOR_OUTLINE)
    --[[font:PrintCenter(vx.screen.width / 2,
            vx.screen.height / 2,
            message)]]
    vx.ShowPage()
end
