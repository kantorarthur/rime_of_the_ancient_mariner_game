#version 410 core

#include "lights.glsl"
#include "fog.glsl"

in vec3 fWorldPosition;
in vec4 fColor;

out vec4 fColor0;

uniform vec3 cameraPosition;

void main()
{
    vec3 toSnake = fWorldPosition - cameraPosition;
    float distance = length(toSnake);
    float transmittance = exp(-fogOpticalDepth(cameraPosition, toSnake / distance, distance));
    fColor0 = vec4(fColor.rgb * 1.8 * mix(0.35, 1.0, transmittance), fColor.a);
}
