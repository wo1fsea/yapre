package.path = package.path .. ";./data/lua/?.lua"

local mario_music = require("mario_music")
local app = require("app")

print("!!!yapre lua!!!")

function Init(str)
    print("Init") 
    app.Init()
    mario_music.play_music()
    return true
end

function Update(delta_ms)
    app.Update(delta_ms)
end

function Deinit()
    print("Deinit")
end
