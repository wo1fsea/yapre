local world_palette = {}

local core = require("core")
local copy = require("utils.copy")
local yecs = core.yecs

function world_palette:Make()
    local world = yecs.World:New("world_palette")
    world:AddSystems({"sprite", "input", "tree", "tick"})

    local palette = yecs.EntityFactory:Make("palette")
    world:AddEntity(palette)
    palette.position = {x=yapre.render_width/2-64, y=yapre.render_height/2, z=1}

    return world
end

return world_palette
