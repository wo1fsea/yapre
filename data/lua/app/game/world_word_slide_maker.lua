local world_word_slide_maker = {}
local world_word_slide = {
    words="",
    font_size=2,
}

local core = require("core")
local copy = require("utils.copy")
local yecs = core.yecs

function world_word_slide:Make()
    local world = yecs.World:New("world_word_slide_"..self.words)
    world:AddSystems({"sprite", "input", "tree", "tick"})

    local words = yecs.EntityFactory:Make("label") 
    words:SetFontSize(self.font_size)
    words:SetText(self.words)

    local rs = words:GetRenderSize()
    words.position = {
        x=yapre.render_width/2-rs.width*self.font_size/2, 
        y=yapre.render_height/2-rs.height*self.font_size/2, 
        z=1
    }

    world:AddEntity(words)

    return world
end

function world_word_slide_maker:Make(words, font_size)
    local new_world = copy.copy(world_word_slide)
    new_world.words = words
    new_world.font_size = font_size
    return new_world
end

return world_word_slide_maker
