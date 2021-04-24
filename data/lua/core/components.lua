local components = {}
local yecs = require("core.yecs")

yecs.Component:Register("position", {x=0, y=0, z=0})
yecs.Component:Register("size", {width=0, height=0})
yecs.Component:Register("sprite", 
{
    sprites={}, 
    AddSprite=function(self, key, texture, params)
        params = params or {}
        self.sprites[key] = {
            texture = texture or "data/image/sprite16.png", 
            color = params.color or {r=1,g=1,b=1}, 
            size = params.size or {width=-1, height=-1}, 
            offset = params.offset or {x=0, y=0, z=0},
        }
    end,
    RemoveSprite=function(self, key)
        self.sprites[key] = nil
    end,
})

yecs.Component:Register("input", 
{
    touch_size=nil,
    touched=false,
    transparent=false,
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
}
)

yecs.Component:Register("tree", 
{
    parent=nil, 
    children={},
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
})

local font_data = require("core.data.font_data")
yecs.Component:Register("text", 
{
    _text="",
    _size=1,
    _max_width=0,
    _max_height=0,
    SetText=function(self, new_text)
        self._text = new_text
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
        local size = self._size
        self._text:gsub(".", function(c)
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
            
            if self._max_width > 0 and pos_x + width > self._max_width then
                pos_y = pos_y + font_data.height + 1
                pos_x = 0
            end

            if self._max_height > 0 and pos_y + font_data.height > self._max_height then 
                return 
            end

            local texture = {
                texture=string.format("data/image/font/%d.png", c_n), 
                color={r=1,g=1,b=1}, 
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
        self._max_width = w
        self._max_height = h
        self:Format()
    end

})

local default_tick_order = 1
yecs.Component:Register("tick", 
{
    callbacks={},
    AddCallback=function(self, key, callback, tick_order)
        self.callbacks[key] = {
            tick_order=tick_order or default_tick_order,
            callback=callback,
            
            component=self,
            entity=self.entity,
        }
    end,
    RemoveCallback=function(self, key)
        self.callbacks[key] = nil
    end,
})

local animation_callback_key = 1
yecs.Component:Register("animation", 
{
    _waiting_timer=false,
    _next_idx={},
    _callback=function(self)
        self._waiting_timer = false
        local entity = self.entity
        local sprite = entity.sprite
        if not sprite then
            self._next_idx = {}
            return 
        end
        
        local next_timer = false
        local next_idx = {}
        for k, idx in pairs(self._next_idx) do
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
            yapre.AddTimer(1000//12, function() self:_callback() end)
            self._waiting_timer = true
        end
        
        self._next_idx = next_idx 
    end,

    frame_rate=12,
    animations={},
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

       self._next_idx[key] = a.start_idx

       if not self._waiting_timer then
           yapre.AddTimer(1000//12, function() self:_callback() end)
           self._waiting_timer = true
       end
   end,
    Stop=function(self, key)
        self._next_idx[key] = nil
    end,
})

return components
