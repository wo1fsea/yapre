local yecs = require("core.yecs")

-- label
local label_behavior = {}
yecs.Behavior:Register("label", label_behavior)
function label_behavior:SetText(text)
    self.text:SetText(text)
end
function label_behavior:SetMaxSize(width, height)
    self.text:SetMaxSize(width, height)
end
function label_behavior:SetFontSize(size)
    self.text:SetFontSize(size)
end
function label_behavior:GetRenderSize()
    return self.text:GetRenderSize()
end

yecs.EntityFactory:Register("label", {"position", "tree", "size", "text", "layout"}, {"label"})
