$input v_color0, v_texcoord0

#include <bgfx_shader.sh>

void main() {
    gl_FragColor = vec4(1.0 * v_texcoord0.x, 1.0 * v_texcoord0.y, 0, 1.0);
}
