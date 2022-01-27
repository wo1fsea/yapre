local yapre = yapre

local yecs = require("core.yecs")

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
