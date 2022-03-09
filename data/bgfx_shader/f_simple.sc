$input v_color0, v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(spriteTex,  0);
uniform vec4 spriteColor;


void main() {
    gl_FragColor = texture2D(spriteTex, v_texcoord0) * spriteColor;
}
