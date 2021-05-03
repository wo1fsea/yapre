local test_world = {}

local core = require("core")
local yecs = core.yecs

function test_world:New()
    local world = yecs.World:New("test_world")
    world:AddSystems({"sprite", "input", "tree", "tick"})

    local button1 = yecs.EntityFactory:Make("button")
    local button2 = yecs.EntityFactory:Make("button")

    local label = yecs.EntityFactory:Make("label")
    local image = yecs.EntityFactory:Make("image")
    local progress = yecs.EntityFactory:Make("progress")
    local panel = yecs.EntityFactory:Make("panel")
    local palette = yecs.EntityFactory:Make("palette")
    
    panel:SetSize(320, 128)
    panel.position = {x=0, y=0, z=0}

    progress.position = {x=80, y=8, z=1}
    progress:SetPercent(10)
    progress:AddComponent("tick")
    
    local progress_delta = 1
    progress.tick:AddTick("increase_progress", 
    function() 
        if progress.percent == 100 then 
            progress_delta = -1
        elseif progress.percent == 0 then
            progress_delta = 1
        end

        progress:SetPercent(progress.percent+progress_delta)
    end)
    
    image:SetTextureSize(128,128)
    image:SetTexture("data/image/animation/blood/1.png")

    image.position = {x=8, y=240-8-128, z=1}
    image:AddAnimationState("die", "data/image/animation/blood/%d.png", 1, 20)
    image:AddAnimationState("live", "data/image/animation/blood/%d.png", 1, 1)

    label:SetText("O wonder! \nHow many goodly creatures are there here!\nHow beauteous mankind is!\nO brave new world,\nThat has such people in't.")
    label:SetMaxSize(64, -1)
    label.position = {x=8, y=8, z=1}

    local image_key = image.key
    local button2_key = button2.key
    yecs.Behavior:Register("button1.B", {
         OnClicked=function(self, x, y)
            yecs.worlds["test_world"].entities[image_key]:PlayAnimation("die")
            yecs.worlds["test_world"].entities[button2_key]:SetState("normal")
            self:SetState("disabled")
        end
    })
    button1:AddBehavior("button1.B")

    button1.position = {x=320-8-32, y=240-8-32, z=1}
    
    function button2.OnClicked(_, x, y)
        image:PlayAnimation("live")
        button2:SetState("disabled")
        button1:SetState("normal")
    end
    button2.position = {x=320-8-32-32, y=240-8-32, z=1}
    
    button1:SetState("normal")
    button2:SetState("disabled")

    world:AddEntity(button1)
    world:AddEntity(button2)
    world:AddEntity(label)
    world:AddEntity(image)
    world:AddEntity(progress)
    world:AddEntity(panel)
    world:AddEntity(palette)

    return world 
end

return test_world
