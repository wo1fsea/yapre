local components = {}
local yecs = require("utils.yecs")

yecs.Component:Register("tree", {parent=nil, children={}})
yecs.Component:Register("position", {x=0, y=0, z=0})
yecs.Component:Register("size", {width=0, height=0})
yecs.Component:Register("sprite", 
{
    sprites={
        {
            texture="data/yapre.png", 
            color={r=1,g=1,b=1}, 
            size={width=-1, heigh=-1}, 
            offset={x=0, y=0, z=0},
        }
    }, 
})

return components
