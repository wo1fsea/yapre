
local core = require("core")
local yecs = core.yecs

local app = {}
local world1 = nil
local button1 = nil
local button2 = nil
local label = nil
local image = nil
local progress = nil
local panel = nil

function app.Init()
    world1 = yecs.World:New("world1")
    world1:AddSystems({"sprite", "input", "tree", "tick"})

    button1 = yecs.EntityFactory:Make("button")
    button2 = yecs.EntityFactory:Make("button")
    
    label = yecs.EntityFactory:Make("label")
    image = yecs.EntityFactory:Make("image")
    progress = yecs.EntityFactory:Make("progress")
    panel = yecs.EntityFactory:Make("panel")
    panel:SetSize(128, 128)
    panel.position = {x=8, y=8, z=0}

    progress.position = {x=80, y=8, z=0}
    progress:SetPercent(10)
    
    image:SetTextureSize(128,128)
    image:SetTexture("data/image/animation/blood/1.png")

    image.position = {x=8, y=240-8-128, z=0}
    image:AddAnimationState("die", "data/image/animation/blood/%d.png", 1, 20)
    image:AddAnimationState("live", "data/image/animation/blood/%d.png", 1, 1)

    label:SetText("O wonder! \nHow many goodly creatures are there here!\nHow beauteous mankind is!\nO brave new world,\nThat has such people in't.")
    label:SetMaxSize(64, -1)
    label.position = {x=8, y=8, z=1}

    function button1:OnClicked(x, y)
        image:PlayAnimation("die")
        self:SetState("disabled")
        button2:SetState("normal")
    end
    button1.position = {x=320-8-32, y=240-8-32, z=0}
    
    function button2:OnClicked(x, y)
        image:PlayAnimation("live")
        self:SetState("disabled")
        button1:SetState("normal")
    end
    button2.position = {x=320-8-32-32, y=240-8-32, z=0}
    
    button1:SetState("normal")
    button2:SetState("disabled")

    world1:AddEntity(button1)
    world1:AddEntity(button2)
    world1:AddEntity(label)
    world1:AddEntity(image)
    world1:AddEntity(progress)
    world1:AddEntity(panel)
end

local p = 0
local delta_i = 1
function app.Update(delta_ms)
    world1:Update(delta_ms)
    p = p + delta_i 
    if p == 100 or p == 0 then delta_i = -1 * delta_i end
    progress:SetPercent(p)
end

function app.Deinit()
end

return app
