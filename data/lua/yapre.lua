package.path = package.path .. ";./data/lua/?.lua"

local mario_music = require("mario_music")
local app = require("app")

print("!!!yapre lua!!!")

function OnKey(timestamp, state, multi, keyode)
    print(string.format("[OnKey] %i:%i:%i:%c", timestamp, state, multi, keyode))
end

function OnMouse(timestamp, state, button, x, y)
    print(string.format("[OnMouse] %i:%i:%i:(%i,%i)", timestamp, state, button, x, y))
end

function Init(str)
    print("Init") 
    app.Init()
    yapre.BindKeyboardInputCallback("OnKey", OnKey)
    yapre.BindMouseInputCallback("OnMouse", OnMouse)
    mario_music.play_music()
    return true
end

function Update()
    app.Update()
end

function Deinit()
    print("Deinit")
    yapre.UnbindKeyboardInputCallback("OnKey")
    yapre.UnbindMouseInputCallback("OnMouse")
end
