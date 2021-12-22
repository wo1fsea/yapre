local emscripten_keycode_mapping = {}

emscripten_keycode_mapping.keycode_mapping = {
    [53] = 96,
    [30] = 49,
    [31] = 50,
    [32] = 51,
    [33] = 52,
    [34] = 53,
    [35] = 54,
    [36] = 55,
    [37] = 56,
    [38] = 57,
    [39] = 48,
    [46] = 61,
    [42] = 8,
    [43] = 9,
    [20] = 113,
    [26] = 119,
    [8] = 101,
    [21] = 114,
    [23] = 116,
    [28] = 121,
    [24] = 117,
    [12] = 105,
    [18] = 111,
    [19] = 112,
    [47] = 91,
    [48] = 93,
    [49] = 92,
    [4] = 97,
    [22] = 115,
    [7] = 100,
    [9] = 102,
    [10] = 103,
    [11] = 104,
    [13] = 106,
    [14] = 107,
    [15] = 108,
    [51] = 59,
    [52] = 39,
    [40] = 13,
    [29] = 122,
    [27] = 120,
    [6] = 99,
    [25] = 118,
    [5] = 98,
    [17] = 110,
    [16] = 109,
    [54] = 44,
    [55] = 46,
    [56] = 47,
    [44] = 32,
    [101] = 231
}

function emscripten_keycode_mapping:GetKeyCode(key_code)
    local n_key_code = self.keycode_mapping[key_code] or key_code
    return n_key_code
end

return emscripten_keycode_mapping
