
local core = require("core")
local yecs = core.yecs

local app = {}
local world1 = nil
local button1 = nil
local button2 = nil
local lable = nil
local image = nil

function app.Init()
    world1 = yecs.World:New("world1")
    world1:AddSystems({"sprite", "input", "tree", "tick"})

    button1 = yecs.EntityFactory:Make("button")
    button2 = yecs.EntityFactory:Make("button")
    
    lable = yecs.EntityFactory:Make("lable")
    image = yecs.Entity:New({"position", "sprite", "size", "animation"})
    
    image.sprite:AddSprite("anim", "data/image/animation/blood/1.png", {size={width=128, height=128}})
    image.position = {x=8, y=240-8-128, z=0}
    image.animation:AddState("die", "anim",  "data/image/animation/blood/%d.png", 1, 20)
    image.animation:AddState("live", "anim",  "data/image/animation/blood/%d.png", 1, 1)

    lable.text:SetText("O wonder! \nHow many goodly creatures are there here!\nHow beauteous mankind is!\nO brave new world,\nThat has such people in't.")
    lable.position = {x=8, y=8, z=0}
    lable.text:SetMaxSize(64, -1)

    function button1.input:OnClicked(x, y)
        image.animation:Play("die")
        self.entity.animation:Stop("normal")
        self.entity.animation:Stop("pressed")
        self.entity.animation:Play("disabled")
    end
    button1.position = {x=320-8-32, y=240-8-32, z=0}
    
    function button2.input:OnClicked(x, y)
        image.animation:Play("live")
        self.entity.animation:Stop("normal")
        self.entity.animation:Stop("pressed")
        self.entity.animation:Play("disabled")
    end
    button2.position = {x=320-8-32-32, y=240-8-32, z=0}

    world1:AddEntity(button1)
    world1:AddEntity(button2)
    world1:AddEntity(lable)
    world1:AddEntity(image)
end

function app.Update(delta_ms)
    world1:Update(delta_ms)
end

function app.Deinit()
end

return app
