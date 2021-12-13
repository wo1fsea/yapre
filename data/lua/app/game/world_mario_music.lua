local yapre = yapre
local world_mario_music = {}

local core = require("core")
local yecs = core.yecs

world_mario_music.data = {{0, 659, 125}, {125, 659, 125}, {375, 659, 125}, {667, 523, 125}, {792, 659, 125},
                          {1042, 784, 125}, {1542, 392, 125}, {2042, 523, 125}, {2417, 392, 125}, {2792, 330, 125},
                          {3167, 440, 125}, {3417, 494, 125}, {3667, 466, 125}, {3834, 440, 125}, {4084, 392, 125},
                          {4334, 659, 125}, {4584, 784, 125}, {4834, 880, 125}, {5084, 698, 125}, {5209, 784, 125},
                          {5459, 659, 125}, {5709, 523, 125}, {5959, 587, 125}, {6084, 494, 125}, {6334, 523, 125},
                          {6709, 392, 125}, {7084, 330, 125}, {7459, 440, 125}, {7709, 494, 125}, {7959, 466, 125},
                          {8126, 440, 125}, {8376, 392, 125}, {8626, 659, 125}, {8876, 784, 125}, {9126, 880, 125},
                          {9376, 698, 125}, {9501, 784, 125}, {9751, 659, 125}, {10001, 523, 125}, {10251, 587, 125},
                          {10376, 494, 125}, {10876, 784, 125}, {11001, 740, 125}, {11126, 698, 125}, {11293, 622, 125},
                          {11543, 659, 125}, {11835, 415, 125}, {11960, 440, 125}, {12085, 523, 125}, {12335, 440, 125},
                          {12460, 523, 125}, {12585, 587, 125}, {12960, 784, 125}, {13085, 740, 125}, {13210, 698, 125},
                          {13377, 622, 125}, {13627, 659, 125}, {13919, 698, 125}, {14169, 698, 125}, {14294, 698, 125},
                          {15044, 784, 125}, {15169, 740, 125}, {15294, 698, 125}, {15461, 622, 125}, {15711, 659, 125},
                          {16003, 415, 125}, {16128, 440, 125}, {16253, 523, 125}, {16503, 440, 125}, {16628, 523, 125},
                          {16753, 587, 125}, {17128, 622, 125}, {17503, 587, 125}, {17878, 523, 125}, {19128, 784, 125},
                          {19253, 740, 125}, {19378, 698, 125}, {19545, 622, 125}, {19795, 659, 125}, {20087, 415, 125},
                          {20212, 440, 125}, {20337, 523, 125}, {20587, 440, 125}, {20712, 523, 125}, {20837, 587, 125},
                          {21212, 784, 125}, {21337, 740, 125}, {21462, 698, 125}, {21629, 622, 125}, {21879, 659, 125},
                          {22171, 698, 125}, {22421, 698, 125}, {22546, 698, 125}, {23296, 784, 125}, {23421, 740, 125},
                          {23546, 698, 125}, {23713, 622, 125}, {23963, 659, 125}, {24255, 415, 125}, {24380, 440, 125},
                          {24505, 523, 125}, {24755, 440, 125}, {24880, 523, 125}, {25005, 587, 125}, {25380, 622, 125},
                          {25755, 587, 125}, {26130, 523, 125}}

yecs.Behavior:Register("world_mario_music_play_btn_behavior", {
    OnClicked = function(self, x, y)
        local data = self.data
        if data.playing then
            return
        end
        data.playing = true
        data.delta_ms = 0

        self.world:GetEntityByTags({"world_mario_music_stop_btn"}):SetState("normal")
        self:SetState("disabled")
    end,
    OnInit = function(self)
        self:AddComponent("tags")
        self:AddComponent("data")
        self:AddComponent("tick")
        local data = self.data
        data.playing = false
        data.delta_ms = 0

        self.tags["world_mario_music_play_btn"] = true
        self.tick:AddTick("world_mario_music_play", function(tick_ms)
            if not data.playing then
                self.world:GetEntityByTags({"world_mario_music_stop_btn"}):SetState("disabled")
                self:SetState("normal")
                return
            end

            self.world:GetEntityByTags({"world_mario_music_stop_btn"}):SetState("normal")
            self:SetState("disabled")

            for _, md in ipairs(world_mario_music.data) do
                if md[1] > data.delta_ms + tick_ms then
                    break
                elseif md[1] > data.delta_ms then
                    yapre.Beep(md[2], md[3])
                    break
                end
            end
            data.delta_ms = data.delta_ms + tick_ms
        end)

    end
})

yecs.Behavior:Register("world_mario_music_stop_btn_behavior", {
    OnClicked = function(self, x, y)
        local play_btn = self.world:GetEntityByTags({"world_mario_music_play_btn"})
        play_btn:SetState("normal")
        play_btn.data.playing = false
        play_btn.data.delta_ms = 0
        self:SetState("disabled")
    end,
    OnInit = function(self)
        self:AddComponent("tags")
        self.tags["world_mario_music_stop_btn"] = true
    end
})

function world_mario_music:Make()
    local world = yecs.World:New("world_mario_music")
    world:AddSystems({"sprite", "input", "tree", "tick"})

    local play_btn = yecs.EntityFactory:Make("button", {"world_mario_music_play_btn_behavior"})
    local stop_btn = yecs.EntityFactory:Make("button", {"world_mario_music_stop_btn_behavior"})
    local image = yecs.EntityFactory:Make("image")
    image:SetTextureSize(32, 32)
    image:SetTexture("./image/mario.png")

    play_btn.position = {
        x = yapre.render_width / 2 - 32,
        y = yapre.render_height / 2,
        z = 1
    }
    stop_btn.position = {
        x = yapre.render_width / 2,
        y = yapre.render_height / 2,
        z = 1
    }
    image.position = {
        x = yapre.render_width / 2 - 16,
        y = yapre.render_height / 2 - 32 - 8,
        z = 1
    }

    world:AddEntity(play_btn)
    world:AddEntity(stop_btn)
    world:AddEntity(image)

    return world
end

return world_mario_music
