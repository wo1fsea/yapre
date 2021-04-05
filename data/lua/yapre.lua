package.path = package.path .. ";./data/lua/?.lua"

local mario_music = require "mario_music"
print("!!!yapre lua!!!")

function OnKey(timestamp, state, multi, keyode)
    print(string.format("[OnKey] %i:%i:%i:%q", timestamp, state, multi, keyode))
end

function OnMouse(timestamp, state, button, x, y)
    print(string.format("[OnMouse] %i:%i:%i:(%i,%i)", timestamp, state, button, x, y))
end

function Init(str)
    print("Init") 
    yapre.BindKeyboardInputCallback("OnKey", OnKey)
    yapre.BindMouseInputCallback("OnMouse", OnMouse)
    mario_music.play_music()
    return true
end

function Update()
    yapre.DrawSprite(
    "data/yapre.png", 
    (320-128)/2, (240-128)/2, 0, 
    128, 128, 
    0., 
    1., 1., 1.
    )
end

function Deinit()
    print("Deinit")
    yapre.UnbindKeyboardInputCallback("OnKey")
    yapre.UnbindMouseInputCallback("OnMouse")
end
