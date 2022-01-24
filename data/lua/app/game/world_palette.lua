local world_palette = {}

local core = require("core")
local yecs = core.yecs

function world_palette:Make()
    local world = yecs.World:New("world_palette")
    world:AddSystems({"sprite", "input", "tree", "tick", "layout"})

    local palette = yecs.EntityFactory:Make("palette")
    world:AddEntity(palette)
    palette.layout:SetCenterX(world:GetRoot().layout:GetCenterX(), 0)
    palette.layout:SetCenterY(world:GetRoot().layout:GetCenterY(), 0)

    return world
end

return world_palette
