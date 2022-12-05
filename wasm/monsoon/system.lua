
package.path = package.path .. ";code/?.lua;code/?/init.lua;code/mungo/?.lua"

-- Initialize verge internal bindings.
require "vx"
-- Mungo, an associative data language.
require "mungo"
-- Initialize the game engine
require "code"

function DistortBlit(x, y, distort, lucent, img, dest)
    local tmp = vx.Image(img.width, 1)
    for i = 1, img.height do
        vx.SetLucent(0)
        tmp:RectFill(0, 0, tmp.width, tmp.height, vx.RGB(255, 0, 255))
        img:Blit(0, 1 - i, tmp)
        vx.SetLucent(lucent)
        tmp:ScaleBlit(x + vx.Random(-distort, distort), y + i - 1, tmp.width, 1 + vx.Random(0, distort), dest)
    end
    vx.SetLucent(0)
end

function ParseConfig()
    local f = vx.File("verge.cfg", vx.FileMode.Read)
    while not f.eof do
        local s = f:ReadLine()
        if not s:startsWith("#") then       
            local name = s:GetToken(" \t", 0)
            local value = s:GetToken(" \t", 1)
            if #name > 0 then
                if name:startsWith("key_") then
                    local bn = name:GetToken("_", 1):gsub("^%l", string.upper)
                    table.insert(button[bn].inputs, vx.key[value])
                elseif name:startsWith("jb_") then
                    local bn = name:GetToken("_", 1):gsub("^%l", string.upper)
                    table.insert(button[bn].inputs, vx.joystick[string.lower(value)] or vx.joystick.button[tonumber(value)])
                end
            end
        end
    end
    f:Close()
end

function autoexec()
    vx.SetResolution(320, 240)
    vx.SetAppName("Molasses Monsoon")
    
    ParseConfig()
    
    Startup()
end
