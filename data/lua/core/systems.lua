local systems = {}
local yecs = require("core.yecs")

-- dummy system
local dummy_system = {}
dummy_system.update_order = 0
function dummy_system:Init()
    print("dummy Init")
end

function dummy_system:Deinit()
    print("dummy Deinit")
end

function dummy_system:Update(delta_ms)
    print("dummy Update ", delta_ms)
end
yecs.System:Register("dummy", dummy_system)


-- sprite system
local sprite_system = {}
sprite_system.update_order = 2048
function sprite_system:Update(delta_ms)
    local tree_system = self.world.systems["tree"]
    local sprite_entities = self.world:GetEntities(function(entity) return entity.sprite end)
    for _, entity in pairs(sprite_entities) do
        local position = entity.position
        if tree_system then
            position = tree_system:GetPosition(entity)
        end
        position = position or {x=0, y=0, z=0}

        for _, sprite_data in pairs(entity.sprite.sprites) do
            local offset = sprite_data.offset
            local size = sprite_data.size
            local color = sprite_data.color
            yapre.DrawSprite(
            sprite_data.texture, 
            position.x + offset.x, position.y + offset.y, position.z + offset.z, 
            size.width, size.height, 
            0., 
            color.r, color.g, color.b 
            ) 
        end
    end
end
yecs.System:Register("sprite", sprite_system)

-- input system
local input_system = { key_events={}, mouse_events={} }

function input_system:Update(delta_ms)
    local input_entities = self.world:GetEntities(function(entity) return entity.input end)
    local tree_system = self.world.systems["tree"]

    for _, mouse_event in ipairs(self.mouse_events) do
        if mouse_event.button ~= 1 then
            goto continue0
        end

        for _, entity in pairs(input_entities) do
            local einput = entity.input

            if mouse_event.state==1 then
                local size = einput.touch_size or entity.size
                if size == nil then goto continue1 end

                local position = nil
                if tree_system then
                    position = tree_system:GetPosition(entity)
                end

                position = position or entity.position
                if position == nil then goto continue1 end
                local x = position.x
                local y = position.y
                local w = size.width
                local h = size.height

                if  mouse_event.x > x and mouse_event.x < x + w and
                    mouse_event.y > y and mouse_event.y < y + h then
                    if einput:_OnTouchBegan(mouse_event.x, mouse_event.y) then
                        einput.touched = true
                        if not einput.transparent then
                            break 
                        end
                    end
                end
            elseif mouse_event.state==2 then
                if einput.touched then 
                    einput:_OnTouchEnded(mouse_event.x, mouse_event.y)
                    einput.touched = false
                end
            elseif mouse_event.state==4 then
                if einput.touched then 
                    einput:_OnTouchMoved(mouse_event.x, mouse_event.y)
                end
            end
            ::continue1::
        end
        ::continue0::
    end

    self.key_events={}
    self.mouse_events={}
end

function input_system:Init()
    local function OnKey(timestamp, state, multi, keycode)
        print(string.format("%s-[OnKey] %i:%i:%i:%c", self.world, timestamp, state, multi, keycode))
        table.insert(self.key_events, {timestamp=timestamp, state=state, multi=multi, keycode=keycode})
        if self.OnKey then
            self:OnKey(timestamp, state, multi, keycode)
        end
    end

    local function OnMouse(timestamp, state, button, x, y)
        print(string.format("%s-:[OnMouse] %i:%i:%i:(%i,%i)", self.world, timestamp, state, button, x, y))
        table.insert(self.mouse_events, {timestamp=timestamp, state=state, button=button, x=x, y=y})
        if self.OnMouse then
            self:OnMouse(timestamp, state, button, x, y)
        end
    end

    yapre.BindKeyboardInputCallback(string.format("%s-OnKey", self.world), OnKey)
    yapre.BindMouseInputCallback(string.format("%s-OnMouse", self.world), OnMouse)
end

function input_system:Deinit()
    yapre.UnbindKeyboardInputCallback(string.format("%s-OnKey", self.world))
    yapre.UnbindMouseInputCallback(string.format("%s-OnMouse", self.world))
end

yecs.System:Register("input", input_system)

-- tree_system
local tree_system = {}
tree_system.update_order = sprite_system.update_order - 1
tree_system.global_position = {}

function tree_system:_UpdateTreeNodePos(node, parent_pos)
    local pos = node.position
    local node_pos = {
        x=pos.x+parent_pos.x, 
        y=pos.y+parent_pos.y, 
        z=pos.z+parent_pos.z, 
    }
    self.global_position[node.key] = node_pos

    for _, c in pairs(node.tree.children) do
        self:_UpdateTreeNodePos(c, node_pos)
    end
end

function tree_system:Update(delta_ms)
    self.global_position = {}
    local world = self.world
    local tree_entities = self.world:GetEntities(function(entity) return entity.tree end)
    for _, entity in pairs(tree_entities) do
        local parent = entity.tree.parent
        if parent == nil then
            self:_UpdateTreeNodePos(entity, {x=0, y=0, z=0})
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

function tree_system:GetPosition(entity)
    local position = self.global_position[entity.key]
    if position then 
        return position 
    else
        return entity.position
    end
end

yecs.System:Register("tree", tree_system)

-- tick system
local tick_system = {}
tick_system.update_order = 0
function tick_system:Init()
end

function tick_system:Deinit()
end

function tick_system:Update(delta_ms)
    local world = self.world
    local tick_entities = self.world:GetEntities(function(entity) return entity.tick end)
    local g_callbacks = {}
    for _, entity in pairs(tick_entities) do
        local etick = entity.tick
        for _, callback in pairs(etick.callbacks) do
            table.insert(g_callbacks, callback)
        end
    end

    table.sort(
    g_callbacks, 
    function(t1, t2)
        if t1.tick_order < t2.tick_order then
            return true
        elseif t1.tick_order > t2.tick_order  then
            return false
        else
            return tonumber(t1.entity.key) < tonumber(t2.entity.key)
        end
    end
    )

    for _, callback in ipairs(g_callbacks) do
        callback.callback(callback.component, delta_ms)
    end
end

yecs.System:Register("tick", tick_system)


return systems
