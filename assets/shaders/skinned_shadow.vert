#version 410 core

layout(location = 0) in vec3 vPosition;
layout(location = 2) in vec2 vTexCoords;

#include "skinning.glsl"

out vec2 fTexCoords;

uniform mat4 model;
uniform mat4 lightSpace;

void main()
{
    fTexCoords = vTexCoords;
    gl_Position = lightSpace * model * skinMatrix() * vec4(vPosition, 1.0);
}
