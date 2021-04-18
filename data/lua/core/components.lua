local components = {}
local yecs = require("utils.yecs")

yecs.Component:Register("tree", {parent=nil, children={}})
yecs.Component:Register("position", {x=0, y=0, z=0})
yecs.Component:Register("size", {width=0, height=0})
yecs.Component:Register("sprite", 
{
    sprites={
        {
            texture="data/image/sprite16.png", 
            color={r=1,g=1,b=1}, 
            size={width=-1, heigh=-1}, 
            offset={x=0, y=0, z=0},
        }
    }, 
})

yecs.Component:Register("input", 
{
    touch_size=nil,
    touched=false,
    transparent=false,
    _OnTouchBegan=function(self, x, y)
        self.touched = true
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

return components
