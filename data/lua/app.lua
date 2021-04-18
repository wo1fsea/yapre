
require("core.entities")
require("core.components")
require("core.systems")

local yecs = require("utils.yecs")

local app = {}
local world1 = nil
local entity1 = nil
local entity2 = nil
local button = nil

function app.Init()
    world1 = yecs.World:New("world1")
    world1:AddSystem("sprite")
    world1:AddSystem("input")
    entity1 = yecs.Entity:New({"position", "sprite", "input", "size"})
    entity2 = yecs.Entity:New({"position", "sprite"})
    button = yecs.Entity:New({"position", "sprite", "input", "size"})
    button.sprite.sprites[1] = {
        texture="data/image/ui/button16_0.png", 
        color={r=1,g=1,b=1}, 
        size={width=64, heigh=64}, 
        offset={x=0, y=0, z=0},
    }
    button.size.width = 64
    button.size.height = 64
    function button.input:OnTouchBegan(x, y)
        self.entity.sprite.sprites[1].texture = "data/image/ui/button16_1.png"
    end
    function button.input:OnTouchEnded(x, y)
        self.entity.sprite.sprites[1].texture = "data/image/ui/button16_0.png"
    end
    button.position.x = 64
    button.position.y = 64
    
    world1:AddEntity(entity1)
    world1:AddEntity(entity2)
    world1:AddEntity(button)
end

function app.Update(delta_ms)
    entity2.position.x = (entity2.position.x + delta_ms // 10) % 240 
    world1:Update(delta_ms)
end

function app.Deinit()
end

return app
