local splash_screen = {}

local core = require("core")
local yecs = core.yecs


function splash_screen:Show(callback)
    local world = yecs.World:New("splash_world")
    world:AddSystems({"sprite", "tick"})

    local image = yecs.EntityFactory:Make("image")

    local image_size = 1

    while image_size < yapre.render_height / 2 do
        image_size = image_size * 2
    end
    
    local image_w = image_size * 2
    local image_h = image_size
    local image_x = yapre.render_width / 2 - image_w / 2
    local image_y = yapre.render_height / 2 - image_h / 2
    
    image:SetTexture("./image/splash.png")
    image:SetTextureSize(image_w, image_h)
    image:SetBorderEnabled(false)
    image.position = {x=image_x, y=image_y, z=1}
    
    world:AddEntity(image)

    image:AddComponent("tick")
    image.tick:AddTimer("timer", 2000, 
    function()
        world:Destroy()
        local c = yapre.palette.background_color
        yapre.SetClearColor(c.r, c.g, c.b, c.a)
        if callback then callback() end
    end
    )
    
    for idx=1, 16, 1 do
        image.tick:AddTimer("timer_" .. idx, 100*idx, 
        function()
            local c = yapre.palette.colors[idx]
            yapre.SetClearColor(c.r, c.g, c.b, c.a)
        end
        )
    end

    return world
end

return splash_screen
