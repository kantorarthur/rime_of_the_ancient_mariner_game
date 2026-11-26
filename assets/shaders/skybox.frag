#version 410 core

#include "lights.glsl"
#include "fog.glsl"

in vec3 fDirection;

out vec4 fColor;

uniform samplerCube skybox;
uniform float brightness;
uniform vec3 cameraPosition;

const float SKY_DISTANCE = 1500.0;

void main()
{
    vec3 direction = normalize(fDirection);
    vec3 color = texture(skybox, direction).rgb * brightness;
    fColor = vec4(applyFogAlongRay(color, cameraPosition, direction, SKY_DISTANCE), 1.0);
}
