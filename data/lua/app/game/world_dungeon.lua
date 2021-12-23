local yapre = yapre

local core = require("core")
local yecs = core.yecs

local world_dungeon = {}

function world_dungeon:Make()
    local world = yecs.World:New("world_dungeon")
    world:AddSystemsByKeys({"sprite", "input", "tree", "tick"})

    local joystick = yecs.EntityFactory:Make("joystick")
    
    joystick.position = {
        x = yapre.render_width / 2 - 32,
        y = yapre.render_height / 2 - 32,
        z = 2
    }

    world:AddEntity(joystick)

    return world
end

return world_dungeon
