package.path = package.path .. ";./data/lua/?.lua;./data/lua/?/init.lua"
require("utils.strict")

local app = require("app")

print("!!!yapre lua!!!")

function Init(str)
    print("Init") 
    app:Init()
    return true
end

function Update(delta_ms)
    app:Update(delta_ms)
end

function Deinit()
    print("Deinit")
end
