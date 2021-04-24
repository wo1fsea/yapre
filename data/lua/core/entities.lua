local entities = {}
local yecs = require("core.yecs")

-- ui
-- button
yecs.EntityFactory:Register(
"button",
{"position", "sprite", "tree", "size", "input", "animation"},
function(button)
    button.sprite:AddSprite(
    "button", 
    "data/image/ui/button16/1.png", 
    {size={width=32, height=32}})
    button.size.width = 32
    button.size.height = 32

    button.animation:AddState("normal", "button",  "data/image/ui/button16/%d.png", 1, 1)
    button.animation:AddState("pressed", "button",  "data/image/ui/button16/%d.png", 2, 2)
    button.animation:AddState("disabled", "button",  "data/image/ui/button16/%d.png", 3, 3)
    function button.input:OnTouchBegan(x, y)
        self.entity.animation:Play("pressed")
        return true
    end
    function button.input:OnTouchEnded(x, y)
        self.entity.animation:Play("normal")
        self:OnClicked(x, y)
    end
    button.position.z = 100
    button.position.x = 100
    button.position.y = 100
    return button
end
)

-- lable
yecs.EntityFactory:Register(
"lable",
{"position", "sprite", "tree", "size", "text"},
nil)

return entities
