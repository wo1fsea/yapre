local yapre = yapre

local core = require("core")
local yecs = core.yecs

local world_dungeon = {}

yecs.Behavior:Register("world_dungeon_joystick_behavior", {
    OnJoystick = function(self, x, y)
        local dot = self.world:GetEntityByTags({"dot"})
        dot.data["speed"] = {
            x = x,
            y = y
        }
    end,
})

function world_dungeon:Make()
    local world = yecs.World:New("world_dungeon")
    world:AddSystemsByKeys({"sprite", "input", "tree", "tick"})

    local dot = yecs.EntityFactory:Make("image")
    dot:AddComponents{"tags", "data", "tick", "layout", "tree"}

    dot.tags["dot"] = true
    dot.data["speed"] = {
        x = 0,
        y = 0
    }

    dot.tick:AddTick("tick", function(delta_ms)
        dot.position.x = dot.position.x + delta_ms * dot.data.speed.x / 10
        dot.position.y = dot.position.y + delta_ms * dot.data.speed.y / 10
    end)

    dot:SetTextureSize(2, 2)
    dot.position = {
        x = yapre.render_width / 2,
        y = yapre.render_height / 2,
        z = 2
    }

    local joystick = yecs.EntityFactory:Make("joystick", {"world_dungeon_joystick_behavior"})
    joystick:AddComponents{"layout"}
    joystick.position = {
        x = yapre.render_width / 2 - 32,
        y = yapre.render_height - 64,
        z = 2
    }

    world:AddEntity(joystick)
    world:AddEntity(dot)
    joystick.layout:SetRight(dot.layout:GetLeft(), -10)
    joystick.layout:SetTop(dot.layout:GetTop(), -10)
    return world
end

return world_dungeon
