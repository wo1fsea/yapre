
require("core.entities")
require("core.components")
require("core.systems")

local yecs = require("utils.yecs")

local app = {}
local world1 = nil
local entity1 = nil
local entity2 = nil
local button1 = nil
local button2 = nil
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
    button1 = yecs.Entity:New({"position", "sprite", "input", "size", "tree", "tick"})
    button2 = yecs.Entity:New({"position", "sprite", "input", "size", "tree", "tick"})
    text = yecs.Entity:New({"position", "sprite", "size", "text", "tick"})
    image = yecs.Entity:New({"position", "sprite", "size", "animation"})
    
    image.sprite:AddSprite("anim", "data/image/animation/blood/1.png", {size={width=128, heigh=128}})
    image.position.x = 8
    image.position.y = 240 - 8 - 128
    image.animation:AddState("die", "anim",  "data/image/animation/blood/%d.png", 1, 20)
    image.animation:AddState("live", "anim",  "data/image/animation/blood/%d.png", 1, 1)

    --[[
    image.tick:AddCallback(
    "animation", 
    function(self, ms)
        if self.entity.animation_ms == nil then self.entity.animation_ms = 0 end

        self.entity.animation_ms = self.entity.animation_ms + ms
        if self.entity.animation_ms < 1000/12 then return end
        self.entity.animation_ms = 0

        if self.entity.animation_idx == nil or self.entity.animation_idx == 0 then 
            self.entity.animation_idx = 1 
            self.entity.animation_idx_d = 1 
        end
        if self.entity.animation_idx > 20 then 
            self.entity.animation_idx = 20 
        end
        self.entity.sprite.sprites["anim"].texture= string.format("data/image/animation/blood/%d.png", self.entity.animation_idx)
        self.entity.animation_idx = self.entity.animation_idx + self.entity.animation_idx_d
    end, 
    10
    )
    --]]
    
   -- button.tick:AddCallback("a", function(self, ms) print(button, ms, 10) end, 10)
    --text.tick:AddCallback("c", function(self, ms) print(text, ms, 3) end, 3)

    text.text:SetText("O wonder! \nHow many goodly creatures are there here!\nHow beauteous mankind is!\nO brave new world,\nThat has such people in't.")
    text.position.x = 8
    text.position.y = 8 

    button1.sprite.sprites[1] = {
        texture="data/image/ui/button16_0.png", 
        color={r=1,g=1,b=1}, 
        size={width=32, heigh=32}, 
        offset={x=0, y=0, z=0},
    }
    button1.size.width = 32
    button1.size.height = 32
    function button1.input:OnTouchBegan(x, y)
        self.entity.sprite.sprites[1].texture = "data/image/ui/button16_1.png"
        return true
    end
    function button1.input:OnTouchEnded(x, y)
        self.entity.sprite.sprites[1].texture = "data/image/ui/button16_0.png"
        image.animation:Play("die")
    end
    button1.position.x = 320 - 8 - 32 
    button1.position.y = 240 - 8 - 32
    button1.position.z = 1
    
    button2.sprite.sprites[1] = {
        texture="data/image/ui/button16_0.png", 
        color={r=1,g=1,b=1}, 
        size={width=32, heigh=32}, 
        offset={x=0, y=0, z=0},
    }
    button2.size.width = 32
    button2.size.height = 32
    function button2.input:OnTouchBegan(x, y)
        self.entity.sprite.sprites[1].texture = "data/image/ui/button16_1.png"
        return true
    end
    function button2.input:OnTouchEnded(x, y)
        self.entity.sprite.sprites[1].texture = "data/image/ui/button16_0.png"

        image.animation:Stop("die")
        image.animation:Play("live")
    end
    button2.position.x = 320 - 8 - 32 - 32 
    button2.position.y = 240 - 8 - 32
    button2.position.z = 1

    entity2.tree:AddChild(entity1)

    entity1.position.x = 32
    entity1.position.y = 32
    entity1.position.z = 0
    entity2.position.z = 1

    world1:AddEntity(entity1)
    world1:AddEntity(entity2)
    world1:AddEntity(button1)
    world1:AddEntity(button2)
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
