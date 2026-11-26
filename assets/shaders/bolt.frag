#version 410 core

#include "lights.glsl"
#include "fog.glsl"

in vec3 fWorldPosition;
in float fAcross;

out vec4 fColor;

uniform vec3 cameraPosition;
uniform float boltBrightness;

const vec3 BOLT_COLOR = vec3(0.75, 0.82, 1.0);

void main()
{
    float core = 1.0 - abs(fAcross);
    float intensity = core * core * 6.0 + core * 1.5;

    vec3 toBolt = fWorldPosition - cameraPosition;
    float distance = length(toBolt);
    float transmittance = exp(-0.35 * fogOpticalDepth(cameraPosition, toBolt / distance, distance));

    fColor = vec4(BOLT_COLOR * intensity * boltBrightness * transmittance, 1.0);
}
