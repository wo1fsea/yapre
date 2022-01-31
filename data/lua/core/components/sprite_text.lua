local yapre = yapre

local yecs = require("core.yecs")
local palette_data = require("core.data.palette_data")

local font_data = require("core.data.font_data")
yecs.Component:Register("sprite_text", {
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
