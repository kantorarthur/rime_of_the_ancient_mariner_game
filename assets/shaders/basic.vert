#version 410 core

#include "wings.glsl"

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoords;

out vec3 fWorldPosition;
out vec3 fNormal;
out vec2 fTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main()
{
    vec3 position = vPosition;
    vec3 normal = vNormal;
    flapWings(position, normal);

    vec4 worldPosition = model * vec4(position, 1.0);
    fWorldPosition = worldPosition.xyz;
    fNormal = normalMatrix * normal;
    fTexCoords = vTexCoords;
    gl_Position = projection * view * worldPosition;
}
