local yapre = yapre

local yecs = require("core.yecs")
local copy = require("copy")
local eval = require("eval")

local _nil_pos = {
    x = 0,
    y = 0,
    z = 0
}
local _nil_size = {
    width = 0,
    height = 0
}
yecs.Component:Register("layout", {
    constraints = {},
    _operations = {
        CenterX = function(self, is_parent)
            local position = self.entity.position
            if is_parent or not position then
                position = _nil_pos
            end
            local size = self.entity.size or _nil_size
            return position.x + size.width / 2
        end,
        CenterY = function(self, is_parent)
            local position = self.entity.position
            if is_parent or not position then
                position = _nil_pos
            end
            local size = self.entity.size or _nil_size
            return position.y + size.height / 2
        end,
        Left = function(self, is_parent)
            local position = self.entity.position
            if is_parent or not position then
                position = _nil_pos
            end
            return position.x
        end,
        Right = function(self, is_parent)
            local position = self.entity.position
            if is_parent or not position then
                position = _nil_pos
            end
            local size = self.entity.size or _nil_size
            return position.x + size.width
        end,
        Top = function(self, is_parent)
            local position = self.entity.position
            if is_parent or not position then
                position = _nil_pos
            end
            return position.y
        end,
        Bottom = function(self, is_parent)
            local position = self.entity.position
            if is_parent or not position then
                position = _nil_pos
            end
            local size = self.entity.size or _nil_size
            return position.y + size.height
        end,
        GetCenterX = function(self)
            return {self.entity, "CenterX"}
        end,
        GetCenterY = function(self)
            return {self.entity, "CenterY"}
        end,
        GetLeft = function(self)
            return {self.entity, "Left"}
        end,
        GetRight = function(self)
            return {self.entity, "Right"}
        end,
        GetTop = function(self)
            return {self.entity, "Top"}
        end,
        GetBottom = function(self)
            return {self.entity, "Bottom"}
        end,
        _Unpack = function(self, constraint)
            local t_entity, point = table.unpack(constraint)
            yapre.log.assert(self.entity.world == t_entity.world and t_entity.tree and self.entity.tree and
                                 (self.entity.tree.parent == t_entity or self.entity.tree.parent == t_entity.tree.parent))
            return t_entity, point
        end,
        _SetConstraints = function(self, constraint, offset, c_key, c_string)
            local t_entity, point = self:_Unpack(constraint)
            c_string = string.format(c_string, point)
            self.constraints[c_key] = {c_string, {
                t_entity_key = t_entity.key,
                offset = offset
            }}
        end,
        SetCenterX = function(self, constraint, offset)
            local c_string = "return function(e) return e.t_entity.layout:%s(e.is_parent)+e.offset-e.size.width/2 end"
            self:_SetConstraints(constraint, offset, "h", c_string)
        end,
        SetCenterY = function(self, constraint, offset)
            local c_string = "return function(e) return e.t_entity.layout:%s(e.is_parent)+e.offset-e.size.height/2 end"
            self:_SetConstraints(constraint, offset, "v", c_string)
        end,
        SetLeft = function(self, constraint, offset)
            local c_string = "return function(e) return e.t_entity.layout:%s(e.is_parent)+e.offset end"
            self:_SetConstraints(constraint, offset, "h", c_string)
        end,
        SetRight = function(self, constraint, offset)
            local c_string = "return function(e) return e.t_entity.layout:%s(e.is_parent)+e.offset-e.size.width end"
            self:_SetConstraints(constraint, offset, "h", c_string)
        end,
        SetTop = function(self, constraint, offset)
            local c_string = "return function(e) return e.t_entity.layout:%s(e.is_parent)+e.offset end"
            self:_SetConstraints(constraint, offset, "v", c_string)
        end,
        SetBottom = function(self, constraint, offset)
            local c_string = "return function(e) return e.t_entity.layout:%s(e.is_parent)+e.offset-e.size.height end"
            self:_SetConstraints(constraint, offset, "v", c_string)
        end,
        DoLayout = function(self)
            if not self.constraints then
                return
            end
            for k, c in pairs(self.constraints) do
                local v_string, env = table.unpack(c)
                local t_entity = self.entity.world.entities[env["t_entity_key"]]
                local entity = self.entity
                env = copy.deep_copy(env)
                env["t_entity"] = t_entity
                env["size"] = self.entity.size or _nil_pos
                local valid = false
                if t_entity and self.entity.world == t_entity.world then
                    if self.entity.tree.parent == t_entity then
                        env["is_parent"] = true
                        valid = true
                    elseif self.entity.tree.parent == t_entity.tree.parent then
                        env["is_parent"] = false
                        valid = true
                    end
                end
                if not valid then
                    self.constraints[k] = nil
                else
                    local func = eval.eval(v_string)
                    local v = func(env)

                    if k == "h" then
                        self.entity.position.x = v
                    elseif k == "v" then
                        self.entity.position.y = v
                    end
                end
            end
        end
    }
})
