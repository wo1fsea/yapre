#if __VERSION__ < 110
precision mediump float;
#endif

varying vec2 TexCoords;

uniform sampler2D sprite;
uniform vec3 spriteColor;

void main()
{
    gl_FragColor = vec4(spriteColor, 1.0) * vec4(1.0, 1.0, 1.0, texture2D(sprite, TexCoords).r);
}

