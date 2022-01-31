local yapre = yapre

local world_word_slide_maker = {}
local world_word_slide = {
    words = "",
    font_size = 2
}

local core = require("core")
local copy = require("copy")
local yecs = core.yecs

local panel_width = 320
local panel_height = 240

function world_word_slide:Make()
    local world = yecs.World:New("world_word_slide_" .. self.words)
    world:AddSystemsByKeys({"sprite", "input", "tree", "tick", "layout"})
    
    local canvas = yecs.EntityFactory:Make("panel")
    world:AddEntity(canvas)
    canvas:SetSize(panel_width, panel_height)

    canvas.layout:SetCenterX(world:GetRoot().layout:GetCenterX(), 0)
    canvas.layout:SetCenterY(world:GetRoot().layout:GetCenterY(), 0)

    local words = yecs.EntityFactory:Make("label")
    canvas.tree:AddChild(words)
    
    words:SetFontSize(self.font_size)
    words:SetText(self.words)

    local rs = words:GetRenderSize()
    words.size.width = rs.width
    words.size.height = rs.height

    words.layout:SetCenterX(canvas.layout:GetCenterX(), 0)
    words.layout:SetCenterY(canvas.layout:GetCenterY(), 0)

    return world
end

function world_word_slide_maker:Make(words, font_size)
    local new_world = copy.copy(world_word_slide)
    new_world.words = words
    new_world.font_size = font_size
    return new_world
end

return world_word_slide_maker
