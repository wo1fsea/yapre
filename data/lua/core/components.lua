local yapre = yapre

local yecs = require("core.yecs")
local palette_data = require("core.data.palette_data")
local copy = require("copy")
local eval = require("eval")

local deep_copy = copy.deep_copy

yecs.Component:Register("data", {})
yecs.Component:Register("tags", {})

yecs.Component:Register("position", {
    x = 0,
    y = 0,
    z = 0
})
yecs.Component:Register("size", {
    width = 0,
    height = 0
})
yecs.Component:Register("sprite", {
    sprites = {},
    _operations = {
        AddSprite = function(self, key, texture, params)
            params = params or {}
            self.sprites[key] = {
                texture = texture or "./image/sprite16.png",
                color = deep_copy(params.color) or {
                    r = 1,
                    g = 1,
                    b = 1
                },
                size = deep_copy(params.size) or {
                    width = -1,
                    height = -1
                },
                offset = deep_copy(params.offset) or {
                    x = 0,
                    y = 0,
                    z = 0
                }
            }
        end,
        RemoveSprite = function(self, key)
            self.sprites[key] = nil
        end
    }
})

yecs.Component:Register("input", {
    touch_size = nil,
    touched = false,
    transparent = false,
    _operations = {
        _OnTouchBegan = function(self, x, y)
            yapre.log.info("_OnTouchBegan")
            return self:OnTouchBegan(x, y)
        end,
        _OnTouchMoved = function(self, x, y)
            yapre.log.info("_OnTouchMoved")
            self:OnTouchMoved(x, y)
        end,
        _OnTouchEnded = function(self, x, y)
            yapre.log.info("_OnTouchEnded")
            self:OnTouchEnded(x, y)
            self:OnClicked(x, y)
        end,
        _OnKeyPressed = function(self, keycode)
            yapre.log.info(string.format("_OnKeyPressed: %s", keycode))
            self:OnKeyPressed(keycode)
        end,
        _OnKeyReleased = function(self, keycode)
            yapre.log.info(string.format("_OnKeyReleased: %s", keycode))
            self:OnKeyReleased(keycode)
        end,

        OnClicked = function(self, x, y)
            yapre.log.info("OnClicked")
        end,
        OnTouchBegan = function(self, x, y)
            yapre.log.info("OnTouchBegan")
        end,
        OnTouchMoved = function(self, x, y)
            yapre.log.info("OnTouchMoved")
        end,
        OnTouchEnded = function(self, x, y)
            yapre.log.info("OnTouchEnded")
        end,
        OnKeyPressed = function(self, keycode)
            yapre.log.info(string.format("OnKeyPressed: %s", keycode))
        end,
        OnKeyReleased = function(self, keycode)
            yapre.log.info(string.format("OnKeyReleased: %s", keycode))
        end
    }
})

yecs.Component:Register("tree", {
    parent = nil,
    children = {},
    _operations = {
        AddChild = function(self, c)
            local c_tree = c.tree
            yapre.log.assert(c_tree, "child has not tree component")

            local c_parent = c_tree.parent
            if c_parent then
                c_parent.tree:RemoveChild(c)
            end

            c_tree.parent = self.entity
            self.children[c.key] = c
        end,
        RemoveChild = function(self, c)
            local c_tree = c.tree
            yapre.log.assert(c_tree, "child has not tree component")
            
            if c_tree.parent ~= self.entity then
                return
            end

            c_tree.parent = nil
            self.children[c.key] = nil
        end,
        Serialize = function(self)
            local children = {}
            local data = {
                parent = self.parent and self.parent.key,
                children = children
            }

            for c_key, _ in pairs(self.children) do
                table.insert(children, c_key)
            end
            return data
        end,
        Deserialize = function(self, data)
            local world_entities = self.entity.world.entities
            self.parent = world_entities[data.parent]

            for _, c_key in ipairs(data.children) do
                self.children[c_key] = world_entities[c_key]
            end
        end
    }
})

local _nil_pos = {
    x = 0,
    y = 0,
    z = 0,
}
local _nil_size = {
    width = 0,
    height = 0,
}
yecs.Component:Register("layout", {
    constraints = {},
    _operations = {
        Left = function(self, is_parent)
            local position = self.entity.position
            if is_parent or not position then
                position = _nil_pos
            end
            return position.x
        end,
        Right = function(self, is_parent)
            local position = self.entity.position
            if is_parent or not position then
                position = _nil_pos
            end
            local size = self.entity.size or _nil_size
            return position.x + size.width
        end,
        Top = function(self, is_parent)
            local position = self.entity.position
            if is_parent or not position then
                position = _nil_pos
            end
            return position.y
        end,
        bottom = function(self, is_parent)
            local position = self.entity.position
            if is_parent or not position then
                position = _nil_pos
            end
            local size = self.entity.size or _nil_size
            return position.y + size.height
        end,
        GetLeft = function(self)
            return {self.entity, "Left"}
        end,
        GetRight = function(self)
            return {self.entity, "Right"}
        end,
        GetTop = function(self)
            return {self.entity, "Top"}
        end,
        GetBottom = function(self)
            return {self.entity, "Bottom"}
        end,
        _Unpack = function(self, constraint)
            local t_entity, point = table.unpack(constraint)
            yapre.log.assert(self.entity.world == t_entity.world and t_entity.tree and self.entity.tree and
                       (self.entity.tree.parent == t_entity or self.entity.tree.parent == t_entity.tree.parent))
            return t_entity, point
        end,
        _SetConstraints = function(self, constraint, offset, c_key, c_string)
            local t_entity, point = self:_Unpack(constraint)
            c_string = string.format(c_string, point)
            self.constraints[c_key] = {c_string, {
                t_entity_key = t_entity.key,
                offset = offset
            }}
        end,
        SetLeft = function(self, constraint, offset)
            local c_string = "return function(e) return e.t_entity.layout:%s(e.is_parent)+e.offset end"
            self:_SetConstraints(constraint, offset, "h", c_string)
        end,
        SetRight = function(self, constraint, offset)
            local c_string = "return function(e) return e.t_entity.layout:%s(e.is_parent)+e.offset-e.size.width end"
            self:_SetConstraints(constraint, offset, "h", c_string)
        end,
        SetTop = function(self, constraint, offset)
            local c_string = "return function(e) return e.t_entity.layout:%s(e.is_parent)+e.offset end"
            self:_SetConstraints(constraint, offset, "v", c_string)
        end,
        SetBottom = function(self, constraint, offset)
            local c_string = "return function(e) return e.t_entity.layout:%s(e.is_parent)+e.offset-e.size.height end"
            self:_SetConstraints(constraint, offset, "v", c_string)
        end,
        DoLayout = function(self)
            if not self.constraints then
                return
            end
            for k, c in pairs(self.constraints) do
                local v_string, env = table.unpack(c)
                local t_entity = self.entity.world.entities[env["t_entity_key"]]
                local entity = self.entity
                env = copy.deep_copy(env)
                env["t_entity"] = t_entity
                env["size"] = self.entity.size or {
                    width = 0,
                    height = 0
                }
                local valid = false
                if t_entity and self.entity.world == t_entity.world then
                    if self.entity.tree.parent == t_entity then
                        env["is_parent"] = true
                        valid = true
                    elseif self.entity.tree.parent == t_entity.tree.parent then
                        env["is_parent"] = false
                        valid = true
                    end
                end
                if not valid then
                    self.constraints[k] = nil
                else
                    local func = eval.eval(v_string)
                    local v = func(env)

                    if k == "h" then
                        self.entity.position.x = v
                    elseif k == "v" then
                        self.entity.position.y = v
                    end
                end
            end
        end
    }
})

local font_data = require("core.data.font_data")
yecs.Component:Register("text", {
    text = "",
    size = 1,
    max_width = 0,
    max_height = 0,
    render_width = 0,
    render_height = 0,

    _operations = {
        SetText = function(self, new_text)
            self.text = new_text
            self:Format()
        end,
        GetText = function(self)
            return self.text
        end,
        Format = function(self)
            local sprite = self.entity.sprite
            if sprite == nil then
                return
            end

            local new_sprites = {}
            local pos_x = 0
            local pos_y = 0
            local pos_z = 1
            self.render_width = 0
            self.render_height = 0
            local size = self.size
            self.text:gsub(".", function(c)
                if c == "\n" then
                    pos_y = pos_y + font_data.height + 1
                    pos_x = 0
                    return
                end

                local c_n = string.byte(c)
                local width = font_data.width[c_n]
                if width == nil then
                    c_n = -1
                    width = font_data[-1]
                end

                if self.max_width > 0 and (pos_x + width) * size > self.max_width then
                    pos_y = pos_y + font_data.height + 1
                    pos_x = 0
                end

                if self.max_height > 0 and pos_y + font_data.height > self.max_height then
                    return
                end

                local texture = {
                    texture = string.format("./image/font/%d.png", c_n),
                    color = palette_data.foreground_color,
                    size = {
                        width = font_data.size * size,
                        height = font_data.size * size
                    },
                    offset = {
                        x = pos_x * size,
                        y = pos_y * size,
                        z = pos_z
                    }
                }
                table.insert(new_sprites, texture)
                pos_x = pos_x + width
                pos_z = pos_z + 1

                self.render_width = math.max(pos_x, self.render_width)
            end)

            self.render_height = pos_y
            sprite.sprites = new_sprites
        end,
        SetMaxSize = function(self, w, h)
            self.max_width = w
            self.max_height = h
            self:Format()
        end,
        SetFontSize = function(self, v)
            self.size = v
            self:Format()
        end,
        GetRenderSize = function(self)
            return {
                width = self.render_width,
                height = self.render_height
            }
        end
    }

})

local default_tick_order = 1
yecs.Component:Register("tick", {
    _tick_callbacks = {}, -- callback(delta_ms)
    _timer_callbacks = {}, -- callback(timeover_ms)
    _operations = {
        AddTick = function(self, key, callback, tick_order)
            self._tick_callbacks[key] = {
                tick_order = tick_order or default_tick_order,
                callback = callback,
                entity = self.entity
            }
        end,
        RemoveTick = function(self, key)
            self._tick_callbacks[key] = nil
        end,
        AddTimer = function(self, key, time_ms, callback)
            self._timer_callbacks[key] = {
                time_ms = time_ms,
                callback = callback,
                entity = self.entity
            }
        end,
        RemoveTimer = function(self, key)
            self._timer_callbacks[key] = nil
        end

    }
})

local animation_frame_rate = 12
local animation_callback_key = 1
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

return nil
