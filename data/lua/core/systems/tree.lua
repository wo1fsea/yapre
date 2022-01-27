local yapre = yapre
local yecs = require("core.yecs")

-- tree_system
local tree_system = {}
tree_system.update_order = 2047 -- sprite_system.update_order - 1
tree_system.global_position = {}

function tree_system:_UpdateTreeNodePos(node, parent_pos)
    if node.world ~= self.world then
        if node.world then
            node.world:RemoveEntity(node)
        end
        self.world:AddEntity(node)
    end

    if node.layout then
        node.layout:DoLayout()
    end

    local pos = node.position
    local node_pos = {
        x = pos.x + parent_pos.x,
        y = pos.y + parent_pos.y,
        z = pos.z + parent_pos.z
    }
    self.global_position[node.key] = node_pos

    for _, c in pairs(node.tree.children) do
        self:_UpdateTreeNodePos(c, node_pos)
    end
end

function tree_system:Update(delta_ms)
    self.global_position = {}
    local world = self.world
    local tree_entities = self.world:GetEntitiesWithComponent("tree")
    
    -- update root size
    local root = world:GetRoot()
    root.size.width = yapre.render_width
    root.size.height = yapre.render_height

    for _, entity in pairs(tree_entities) do
        local parent = entity.tree.parent
        if parent == nil then
            self:_UpdateTreeNodePos(entity, {
                x = 0,
                y = 0,
                z = 0
            })
        else
            if world.entities[parent.key] == nil then
                world:RemoveEntity(entity)
            end
        end
    end
end

function tree_system:Init()
end

function tree_system:Deinit()
end

function tree_system:GetGlobalPosition(entity)
    local position = self.global_position[entity.key]
    if position then
        return position
    else
        return entity.position
    end
end

yecs.System:Register("tree", tree_system)