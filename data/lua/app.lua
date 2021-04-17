
require("core.entities")
require("core.components")
require("core.systems")

local yecs = require("utils.yecs")

local app = {}
local world1 = nil

function app.Init()
    world1 = yecs.World:New("world1")
    world1:AddSystem("sprite")
    world1:AddSystem("input")
    entity = yecs.Entity:New({"position", "sprite", "input", "size"})
    world1:AddEntity(entity)
    entity.size.width = 64
    entity.size.height = 64
    entity2 = yecs.Entity:New({"position", "sprite"})
    entity2.position.z = 1
    world1:AddEntity(entity2)
end

function app.Update(delta_ms)
    entity.position.x = (entity.position.x + delta_ms // 10) % 240 
    world1:Update(delta_ms)
end

function app.Deinit()
end

return app
