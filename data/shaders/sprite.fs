#version 150 
#if __VERSION__ < 110
precision mediump float;
#endif

in vec2 TexCoords;
layout(location = 0) out vec3 fragColor;

uniform sampler2D sprite;
uniform vec3 spriteColor;

void main()
{
    fragColor = vec4(spriteColor, 1.0) * texture(sprite, TexCoords);
}


