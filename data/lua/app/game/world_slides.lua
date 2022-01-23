local yapre = yapre
local world_slides = {}

local core = require("core")
local yecs = core.yecs

yecs.Behavior:Register("world_slides_next_btn_behavior", {
    OnInit = function (self)
        self:BindKey("x")
        self:AddComponents{"layout"}
    end,
    OnClicked = function(self, x, y)
        local game = require("app.game")
        if game.NextWorld then
            game:NextWorld()
        end
    end
})

yecs.Behavior:Register("world_slides_prev_btn_behavior", {
    OnInit = function (self)
        self:BindKey("z")
        self:AddComponents{"layout"}
    end,
    OnClicked = function(self, x, y)
        local game = require("app.game")
        if game.PrevWorld then
            game:PrevWorld()
        end
    end
})

function world_slides:Make()
    local world = yecs.World:New("world_slides")
    world:AddSystemsByKeys({"sprite", "input", "tree", "tick", "layout"})

    local next_btn = yecs.EntityFactory:Make("button", {"world_slides_next_btn_behavior"})
    local prev_btn = yecs.EntityFactory:Make("button", {"world_slides_prev_btn_behavior"})

    next_btn.position = {
        x = 0,
        y = 0,
        z = 1024
    }
    prev_btn.position  = {
        x = 0,
        y = 0,
        z = 1024
    }

    world:AddEntity(next_btn)
    world:AddEntity(prev_btn)

    next_btn.layout:SetRight(world:GetRoot().layout:GetRight(), -5)
    next_btn.layout:SetBottom(world:GetRoot().layout:GetBottom(), -2)
    prev_btn.layout:SetRight(next_btn.layout:GetLeft(), 0)
    prev_btn.layout:SetTop(next_btn.layout:GetTop(), 0)

    return world
end

return world_slides
