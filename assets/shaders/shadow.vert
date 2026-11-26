#version 410 core

#include "wings.glsl"

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoords;

out vec2 fTexCoords;

uniform mat4 model;
uniform mat4 lightSpace;

void main()
{
    vec3 position = vPosition;
    vec3 normal = vNormal;
    flapWings(position, normal);

    fTexCoords = vTexCoords;
    gl_Position = lightSpace * model * vec4(position, 1.0);
}
