local components = {}
local yecs = require("core.yecs")
local palette_data = require("core.data.palette_data")

local deep_copy = yecs.deep_copy

yecs.Component:Register("position", {x=0, y=0, z=0})
yecs.Component:Register("size", {width=0, height=0})
yecs.Component:Register("sprite", 
{
    sprites={}, 
    _operations={
        AddSprite=function(self, key, texture, params)
            params = params or {}
            self.sprites[key] = {
                texture = texture or "data/image/sprite16.png", 
                color = deep_copy(params.color) or {r=1,g=1,b=1}, 
                size = deep_copy(params.size) or {width=-1, height=-1}, 
                offset = deep_copy(params.offset) or {x=0, y=0, z=0},
            }
        end,
        RemoveSprite=function(self, key)
            self.sprites[key] = nil
        end,
    },
})

yecs.Component:Register("input", 
{
    touch_size=nil,
    touched=false,
    transparent=false,
    _operations={
        _OnTouchBegan=function(self, x, y)
            print("_OnTouchBegan")
            return self:OnTouchBegan(x, y)
        end,
        _OnTouchMoved=function(self, x, y)
            print("_OnTouchMoved")
            self:OnTouchMoved(x, y)
        end,
        _OnTouchEnded=function(self, x, y)
            print("_OnTouchEnded")
            self:OnTouchEnded(x, y)
            self:OnClicked(x, y)
        end,

        OnClicked=function(self, x, y)
            print("OnClicked")
        end,
        OnTouchBegan=function(self, x, y)
            print("OnTouchBegan")
        end,
        OnTouchMoved=function(self, x, y)
            print("OnTouchMoved")
        end,
        OnTouchEnded=function(self, x, y)
            print("OnTouchEnded")
        end
    },
})

yecs.Component:Register("tree", 
{
    parent=nil, 
    children={},
    _operations={
        AddChild=function(self, c)
            if c.tree == nil then return end
            local c_parent = c.tree.parent
            if c_parent then
                c_parent.tree:RemoveChild(c)
            end

            c.tree.parent = self.entity
            self.children[c.key] = c
        end,
        RemoveChild=function(self, c)
            local c_tree = c.tree
            if c_tree == nil then return end
            if c_tree.parent ~= self.entity then return end

            c_tree.parent = nil
            self.children[c.key] = nil
        end
    },
})

local font_data = require("core.data.font_data")
yecs.Component:Register("text", 
{
    text="",
    size=1,
    max_width=0,
    max_height=0,

    _operations={
        SetText=function(self, new_text)
            self.text = new_text
            self:Format()
        end,
        GetText=function(self)
            return self.text
        end,
        Format=function(self)
            local sprite = self.entity.sprite
            if sprite == nil then return end

            local new_sprites = {}
            local pos_x = 0
            local pos_y = 0
            local pos_z = 1
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

                if self.max_width > 0 and pos_x + width > self.max_width then
                    pos_y = pos_y + font_data.height + 1
                    pos_x = 0
                end

                if self.max_height > 0 and pos_y + font_data.height > self.max_height then 
                    return 
                end

                local texture = {
                    texture=string.format("data/image/font/%d.png", c_n), 
                    color=palette_data.foreground_color, 
                    size={width=font_data.size*size, height=font_data.size*size}, 
                    offset={x=pos_x*size, y=pos_y*size, z=pos_z},
                }
                table.insert(new_sprites, texture)
                pos_x = pos_x + width
                pos_z = pos_z + 1
            end)

            sprite.sprites = new_sprites
        end,
        SetMaxSize=function(self, w, h)
            self.max_width = w
            self.max_height = h
            self:Format()
        end
    },

})

local default_tick_order = 1
yecs.Component:Register("tick", 
{
    _tick_callbacks={}, -- callback(delta_ms)
    _timer_callbacks={}, -- callback(timeover_ms)
    _operations={
        AddTick=function(self, key, callback, tick_order)
            self._tick_callbacks[key] = {
                tick_order=tick_order or default_tick_order,
                callback=callback,
                entity=self.entity,
            }
        end,
        RemoveTick=function(self, key)
            self._tick_callbacks[key] = nil
        end,
        AddTimer=function(self, key, time_ms, callback)
            self._timer_callbacks[key] = {
                time_ms=time_ms,
                callback=callback,
                entity=self.entity,
            }
        end,
        RemoveTimer=function(self, key)
            self._timer_callbacks[key] = nil
        end,

    },
})

local animation_frame_rate = 12
local animation_callback_key = 1
yecs.Component:Register("animation", 
{
    frame_rate=animation_frame_rate,
    waiting_timer=false,
    next_idx={},
    animations={},

    _callback=function(self)
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
                    if idx + 1 < a.end_idx then
                        next_idx[k] = idx + 1
                        next_timer = true
                    end
                end
            end
        end

        if next_timer then
            entity.tick:AddTimer("animation", 1000//self.frame_rate, function() self:_callback() end)
            self.waiting_timer = true
        end

        self.next_idx = next_idx 
    end,

    _operations={
        AddState=function(self, key, sprite_key, texture_format, start_idx, end_idx)
            self.animations[key] = {
                sprite_key=sprite_key,
                texture_format=texture_format,
                start_idx=start_idx,
                end_idx=end_idx,
            }
        end,
        RemoveState=function(self, key)
            animations[key] = nil
        end,
        Play=function(self, key)
            local a = self.animations[key]
            if not a then return end

            local s = self.entity.sprite.sprites[a.sprite_key]
            if not s then return end

            self.next_idx[key] = a.start_idx

            if not self.waiting_timer then
                self.entity.tick:AddTimer("animation", 1000//self.frame_rate, function() self:_callback() end)
                self.waiting_timer = true
            end
        end,
        Stop=function(self, key)
            self.next_idx[key] = nil
        end,
        StopAll=function(self)
            self.next_idx = {}
        end,
    },
})

return components
