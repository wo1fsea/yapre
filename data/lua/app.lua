
require("core.entities")
require("core.components")
require("core.systems")

local yecs = require("utils.yecs")

local app = {}
local world1 = nil

function app.Init()
    world1 = yecs.World:New("world1")
    world1:AddSystem("sprite")
    entity = yecs.Entity:New({"position", "sprite"})
    world1:AddEntity(entity)
    entity2 = yecs.Entity:New({"position", "sprite"})
    entity2.position.z = 1
    world1:AddEntity(entity2)
end

function app.Update()
    entity.position.x = (entity.position.x + 1) % 240 
    world1:Update(0)
end

function app.Deinit()
end

return app
