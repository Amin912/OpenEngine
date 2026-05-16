#version 330 core

in vec3 worldPosition;
in vec3 normal;
in vec2 uv;
out vec4 fragColor;

uniform sampler2D textureA;
uniform sampler2D textureB;
uniform float blend;       // 0 shows textureA, 1 shows textureB
uniform vec3 tint;         // multiplied with the texture colour
uniform vec3 cameraPosition;
uniform vec3 lightDirection;  // direction the light travels, normalised
uniform vec3 skyColor;     // also the fog colour
uniform float shininess;

void main()
{
    vec3 albedo = mix(texture(textureA, uv).rgb, texture(textureB, uv).rgb, blend) * tint;
    vec3 n = normalize(normal);
    vec3 toLight = -lightDirection;
    vec3 toCamera = normalize(cameraPosition - worldPosition);

    // Ambient light from above is sky-coloured, from below ground-coloured.
    vec3 ground = vec3(0.25, 0.22, 0.20);
    vec3 ambient = mix(ground, skyColor, n.y * 0.5 + 0.5) * 0.35;

    // Blinn-Phong: diffuse from the angle to the light, specular from the
    // half-way vector between the light and the camera.
    float diffuse = max(dot(n, toLight), 0.0);
    vec3 halfway = normalize(toLight + toCamera);
    float specular = diffuse > 0.0 ? pow(max(dot(n, halfway), 0.0), shininess) : 0.0;
    vec3 sun = vec3(1.0, 0.96, 0.88);

    vec3 color = albedo * (ambient + sun * diffuse) + sun * specular * 0.35;

    // Fade into the sky with distance.
    float fog = 1.0 - exp(-pow(length(cameraPosition - worldPosition) * 0.035, 2.0));
    color = mix(color, pow(skyColor, vec3(2.2)), fog);

    // Lighting is done in linear space; convert to sRGB for the screen.
    fragColor = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
}
