local yapre = yapre

local yecs = require("core.yecs")

yecs.Component:Register("tree", {
    parent = nil,
    children = {},
    _operations = {
        AddChild = function(self, c)
            local c_tree = c.tree
            yapre.log.assert(c_tree, "child has not tree component")

            local c_parent = c_tree.parent
            if c_parent then
                c_parent.tree:RemoveChild(c)
            end

            if c.world ~= self.entity.world then
                if c.world then
                    c.world:RemoveEntity(c)
                end
            end

            c_tree.parent = self.entity
            self.children[c.key] = c
            if c.world ~= self.entity.world then
                self.entity.world:AddEntity(c)
            end
        end,
        RemoveChild = function(self, c)
            local c_tree = c.tree
            yapre.log.assert(c_tree, "child has not tree component")

            if c_tree.parent ~= self.entity then
                return
            end

            c_tree.parent = nil
            self.children[c.key] = nil
        end,
        Serialize = function(self)
            local children = {}
            local data = {
                parent = self.parent and self.parent.key,
                children = children
            }

            for c_key, _ in pairs(self.children) do
                table.insert(children, c_key)
            end
            return data
        end,
        Deserialize = function(self, data)
            local world_entities = self.entity.world.entities
            self.parent = world_entities[data.parent]

            for _, c_key in ipairs(data.children) do
                self.children[c_key] = world_entities[c_key]
            end
        end
    }
})
