package.path = package.path .. ";./data/lua/?.lua;./data/lua/?/init.lua"
require("utils.strict")

local palette_data = require("core.data.palette_data")
yapre.palette = palette_data

local app = require("app")
yapre.app = app

print("!!!yapre lua!!!")

function Init(str)
    print("Init") 
    return app:Init()
end

function Update(delta_ms)
    app:Update(delta_ms)
end

function Deinit()
    print("Deinit")
    app:Deinit()
end
