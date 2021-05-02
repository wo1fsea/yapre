package.path = package.path .. ";./data/lua/?.lua;./data/lua/?/init.lua"
require("utils.strict")

local app = require("app")
yapre.app = app

local palette_data = require("core.data.palette_data")
yapre.palette = palette_data

local background_color = palette_data.background_color
yapre.SetClearColor(background_color.r, background_color.g, background_color.b, background_color.a)

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
