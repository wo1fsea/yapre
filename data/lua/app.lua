
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
local image = nil

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
    image = yecs.Entity:New({"position", "sprite", "size", "tick"})
    
    image.sprite.sprites[1] = {
        texture="data/image/animation/blood/1.png", 
        color={r=1,g=1,b=1}, 
        size={width=128, heigh=128}, 
        offset={x=0, y=0, z=0},
    }
    image.position.x = 8
    image.position.y = 240 - 8 - 128

    image.tick:AddCallback(
    "animation", 
    function(self, ms)
        if self.entity.animation_ms == nil then self.entity.animation_ms = 0 end

        self.entity.animation_ms = self.entity.animation_ms + ms
        if self.entity.animation_ms < 1000/24 then return end
        self.entity.animation_ms = 0

        if self.entity.animation_idx == nil or self.entity.animation_idx == 0 then 
            self.entity.animation_idx = 1 
            self.entity.animation_idx_d = 1 
        end
        if self.entity.animation_idx > 20 then 
            self.entity.animation_idx = 20 
        end
        print(self.entity.animation_idx)
        self.entity.sprite.sprites[1].texture= string.format("data/image/animation/blood/%d.png", self.entity.animation_idx)
        self.entity.animation_idx = self.entity.animation_idx + self.entity.animation_idx_d
    end, 
    10
    )
    
    button.tick:AddCallback("a", function(self, ms) print(button, ms, 10) end, 10)
    text.tick:AddCallback("c", function(self, ms) print(text, ms, 3) end, 3)

    text.text:SetText("O wonder! \nHow many goodly creatures are there here!\nHow beauteous mankind is!\nO brave new world,\nThat has such people in't.")
    text.position.x = 8
    text.position.y = 8 

    button.sprite.sprites[1] = {
        texture="data/image/ui/button16_0.png", 
        color={r=1,g=1,b=1}, 
        size={width=32, heigh=32}, 
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
        image.animation_idx = 1
    end
    button.position.x = 320 - 8 - 32 
    button.position.y = 240 - 8 - 32
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
    world1:AddEntity(image)
end

function app.Update(delta_ms)
    entity2.position.x = (entity2.position.x + delta_ms // 10) % 320 
    world1:Update(delta_ms)
end

function app.Deinit()
end

return app
