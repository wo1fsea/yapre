local components = {}
local yecs = require("utils.yecs")

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

return components
