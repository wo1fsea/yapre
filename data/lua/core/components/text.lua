local yapre = yapre

local yecs = require("core.yecs")
local palette_data = require("core.data.palette_data")

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
            local render_size = yapre.CalculateTextRenderSize(self.text, self.size, {self.max_width, self.max_height})
            self.render_width = render_size[1]
            self.render_height = render_size[2]
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
