#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 worldPosition;
out vec3 normal;
out vec2 uv;

void main()
{
    vec4 world = model * vec4(aPosition, 1.0);
    worldPosition = world.xyz;
    // The inverse transpose keeps normals perpendicular to the surface even
    // when the model is scaled unevenly.
    normal = mat3(transpose(inverse(model))) * aNormal;
    uv = aUv;
    gl_Position = projection * view * world;
}
