local yecs = require("core.yecs")

-- label
local sprite_label_behavior = {}
yecs.Behavior:Register("sprite_label", sprite_label_behavior)
function sprite_label_behavior:SetText(text)
    self.sprite_text:SetText(text)
end
function sprite_label_behavior:SetMaxSize(width, height)
    self.sprite_text:SetMaxSize(width, height)
end
function sprite_label_behavior:SetFontSize(size)
    self.sprite_text:SetFontSize(size)
end
function sprite_label_behavior:GetRenderSize()
    return self.sprite_text:GetRenderSize()
end

yecs.EntityFactory:Register("sprite_label", {"position", "sprite", "tree", "size", "sprite_text", "layout"}, {"sprite_label"})
