
print("!!!yapre lua!!!")

function Init(str)
    print("Init")
    yapre.PlayMario();
    return true 
end

function Update()
    yapre.DrawSprite(
    "data/yapre.png", 
    0, -40, 0, 
    320, 320, 
    0., 
    1., 1., 1.
    )
end

function Deinit()
    print("Deinit")
end
