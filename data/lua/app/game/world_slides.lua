local slides = {}

local core = require("core")
local yecs = core.yecs


yecs.Behavior:Register("slides_next_btn_behavior", {
    OnClicked=function(self, x, y)
        local game = require("app.game")
        if game.NextWorld then
            game:NextWorld()
        end
    end,
})

yecs.Behavior:Register("slides_prev_btn_behavior", {
    OnClicked=function(self, x, y)
        local game = require("app.game")
        if game.PrevWorld then
            game:PrevWorld()
        end
    end,
})

function slides:Make()
    local world = yecs.World:New("slides")
    world:AddSystems({"sprite", "input", "tree", "tick"})

    local next_btn = yecs.EntityFactory:Make("button", {"slides_next_btn_behavior"})
    local prev_btn = yecs.EntityFactory:Make("button", {"slides_prev_btn_behavior"})

    next_btn.position = {x=yapre.render_width-4-32, y=yapre.render_height-32, z=1}
    prev_btn.position = {x=yapre.render_width-4-32*2, y=yapre.render_height-32, z=1}

    world:AddEntity(next_btn)
    world:AddEntity(prev_btn)

    return world
end

return slides
