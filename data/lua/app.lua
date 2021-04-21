
require("core.entities")
require("core.components")
require("core.systems")

local yecs = require("utils.yecs")

local app = {}
local world1 = nil
local entity1 = nil
local entity2 = nil
local button = nil
local text = nil

function app.Init()
    world1 = yecs.World:New("world1")
    world1:AddSystem("sprite")
    world1:AddSystem("input")
    world1:AddSystem("tree")
    world1:AddSystem("tick")
    entity1 = yecs.Entity:New({"position", "sprite", "input", "size", "tree"})
    entity2 = yecs.Entity:New({"position", "sprite", "tree"})
    button = yecs.Entity:New({"position", "sprite", "input", "size", "tree", "tick"})
    text = yecs.Entity:New({"position", "sprite", "size", "text", "tick"})

    button.tick:AddCallback("a", function(self, ms) print(button, ms, 10) end, 10)
    text.tick:AddCallback("c", function(self, ms) print(text, ms, 3) end, 3)

    text.text:SetText("O wonder! \nHow many goodly creatures are there here!\nHow beauteous mankind is!\nO brave new world,\nThat has such people in't.")
    text.position.x = 16
    text.position.y = 32 

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
        return true
    end
    function button.input:OnTouchEnded(x, y)
        self.entity.sprite.sprites[1].texture = "data/image/ui/button16_0.png"
    end
    button.position.x = 256 
    button.position.y = 128
    button.position.z = 1
    
    entity2.tree:AddChild(entity1)

    entity1.position.x = 32
    entity1.position.y = 32
    entity1.position.z = 0
    entity2.position.z = 1

    world1:AddEntity(entity1)
    world1:AddEntity(entity2)
    world1:AddEntity(button)
    world1:AddEntity(text)
end

function app.Update(delta_ms)
    entity2.position.x = (entity2.position.x + delta_ms // 10) % 320 
    world1:Update(delta_ms)
end

function app.Deinit()
end

return app
