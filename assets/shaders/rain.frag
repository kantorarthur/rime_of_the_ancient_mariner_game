#version 410 core

#include "lights.glsl"
#include "fog.glsl"

in vec3 fWorldPosition;
in float fAlong;
in float fAcross;

out vec4 fColor;

uniform vec3 cameraPosition;
uniform float rainOpacity;

const float PI = 3.14159265;

void main()
{
    float distance = length(fWorldPosition - cameraPosition);
    float shape = (1.0 - abs(fAcross)) * sin(PI * fAlong);
    float fade = smoothstep(0.4, 1.5, distance) * (1.0 - smoothstep(12.0, 24.0, distance));

    vec3 light = fog.color * 5.0 + moon.ambient * 3.0 + moon.color * 0.4;
    for(int i = 0; i < pointLightCount; i++)
    {
        float lightDistance = length(pointLights[i].position - fWorldPosition);
        light += pointLights[i].color * attenuation(lightDistance, pointLights[i].constant, pointLights[i].linear, pointLights[i].quadratic) * 0.6;
    }

    vec3 toBeam = beam.position - fWorldPosition;
    float beamDistance = length(toBeam);
    light += beam.color * beamCone(toBeam / beamDistance) * attenuation(beamDistance, beam.constant, beam.linear, beam.quadratic) * 0.8;

    fColor = vec4(light, rainOpacity * shape * fade);
}
