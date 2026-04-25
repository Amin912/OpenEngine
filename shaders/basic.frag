#version 330 core

in vec2 uv;
out vec4 fragColor;

uniform sampler2D textureA;
uniform sampler2D textureB;
uniform float blend;  // 0 shows textureA, 1 shows textureB

void main()
{
    // Textures are stored as sRGB, so sampling returns linear colour. Convert
    // back to sRGB for the screen.
    vec3 color = mix(texture(textureA, uv).rgb, texture(textureB, uv).rgb, blend);
    fragColor = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
}
