local yapre = yapre

local yecs = require("core.yecs")

local animation_frame_rate = 12
yecs.Component:Register("animation", {
    frame_rate = animation_frame_rate,
    waiting_timer = false,
    next_idx = {},
    animations = {},

    _callback = function(self)
        self.waiting_timer = false
        local entity = self.entity
        local sprite = entity.sprite
        if not sprite then
            self.next_idx = {}
            return
        end

        local next_timer = false
        local next_idx = {}
        for k, idx in pairs(self.next_idx) do
            local a = self.animations[k]
            if a then
                local s = sprite.sprites[a.sprite_key]
                if s then
                    s.texture = string.format(a.texture_format, idx)
                    if idx + 1 <= a.end_idx then
                        next_idx[k] = idx + 1
                        next_timer = true
                    elseif a.loop then
                        next_idx[k] = a.start_idx
                        next_timer = true
                    end
                end
            end
        end

        if next_timer then
            entity.tick:AddTimer("animation", 1000 // self.frame_rate, function()
                self:_callback()
            end)
            self.waiting_timer = true
        end

        self.next_idx = next_idx
    end,

    _operations = {
        AddState = function(self, key, sprite_key, texture_format, start_idx, end_idx, loop)
            self.animations[key] = {
                sprite_key = sprite_key,
                texture_format = texture_format,
                start_idx = start_idx,
                end_idx = end_idx,
                loop = loop
            }
        end,
        RemoveState = function(self, key)
            self.animations[key] = nil
        end,
        Play = function(self, key)
            local a = self.animations[key]
            if not a then
                return
            end

            local s = self.entity.sprite.sprites[a.sprite_key]
            if not s then
                return
            end

            self.next_idx[key] = a.start_idx

            if not self.waiting_timer then
                self.entity.tick:AddTimer("animation", 1000 // self.frame_rate, function()
                    self:_callback()
                end)
                self.waiting_timer = true
            end
        end,
        Stop = function(self, key)
            self.next_idx[key] = nil
        end,
        StopAll = function(self)
            self.next_idx = {}
        end,
        Deserialize = function(self, data)
            yecs.Component.Deserialize(self, data)
            self.entity.tick:AddTimer("animation", 1000 // self.frame_rate, function()
                self:_callback()
            end)
        end

    }
})