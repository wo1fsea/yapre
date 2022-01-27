local yapre = yapre

local yecs = require("core.yecs")
local copy = require("copy")

local deep_copy = copy.deep_copy

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
