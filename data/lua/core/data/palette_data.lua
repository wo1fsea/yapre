local palette_data = {}

local GetColorFromHexStr = function(str)
    local color = {a=0, r=0, g=0, b=0}
    local tmp = {}
    for cc in string.gmatch(str, "..") do
        table.insert(tmp, tonumber(cc, 16))
    end
    color.a = tmp[1] / 255
    color.r = tmp[2] / 255
    color.g = tmp[3] / 255
    color.b = tmp[4] / 255
    return color
end

palette_data.colors = {
    GetColorFromHexStr("FF2497fc"), 
    GetColorFromHexStr("FF283d85"),
    GetColorFromHexStr("FF271f36"),
    GetColorFromHexStr("FF592b66"),
    GetColorFromHexStr("FFa60d0d"),
    GetColorFromHexStr("FFff5a00"),
    GetColorFromHexStr("FFfcb500"),
    GetColorFromHexStr("FFede547"),
    GetColorFromHexStr("FF85d451"),
    GetColorFromHexStr("FF029925"),
    GetColorFromHexStr("FF00401c"),
    GetColorFromHexStr("FF000000"),
    GetColorFromHexStr("FF475370"),
    GetColorFromHexStr("FF798b94"),
    GetColorFromHexStr("FF99cfc5"),
    GetColorFromHexStr("FFebffcc"),
}

return palette_data

