#version 410 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoords;

#include "skinning.glsl"

out vec3 fWorldPosition;
out vec3 fNormal;
out vec2 fTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main()
{
    mat4 skin = skinMatrix();
    vec4 worldPosition = model * skin * vec4(vPosition, 1.0);
    fWorldPosition = worldPosition.xyz;
    fNormal = normalMatrix * mat3(skin) * vNormal;
    fTexCoords = vTexCoords;
    gl_Position = projection * view * worldPosition;
}
