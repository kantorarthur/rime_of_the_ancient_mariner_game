#version 410 core

#include "lights.glsl"
#include "fog.glsl"

in vec3 fWorldPosition;

out vec4 fColor;

uniform vec3 cameraPosition;
uniform float shaftLength;
uniform float shaftStartRadius;
uniform float shaftSpread;
uniform float shaftStrength;
uniform float clearAirDensity;

void main()
{
    vec3 toFragment = fWorldPosition - cameraPosition;
    float rayLength = length(toFragment);
    vec3 rayDirection = toFragment / rayLength;
    vec3 axis = normalize(beam.direction);
    vec3 offset = cameraPosition - beam.position;

    float alignment = dot(rayDirection, axis);
    float rayOffset = dot(rayDirection, offset);
    float axisOffset = dot(axis, offset);
    float denominator = max(1.0 - alignment * alignment, 0.0001);

    float alongAxis = clamp((axisOffset - alignment * rayOffset) / denominator, 0.0, shaftLength);
    float alongRay = clamp(dot(beam.position + axis * alongAxis - cameraPosition, rayDirection), 0.0, rayLength);
    alongAxis = clamp(dot(cameraPosition + rayDirection * alongRay - beam.position, axis), 0.0, shaftLength);

    vec3 pointOnAxis = beam.position + axis * alongAxis;
    vec3 pointOnRay = cameraPosition + rayDirection * alongRay;
    float radius = shaftStartRadius + alongAxis * shaftSpread;
    float core = clamp(1.0 - length(pointOnRay - pointOnAxis) / radius, 0.0, 1.0);
    core *= core;

    float lengthFade = 1.0 - smoothstep(0.6 * shaftLength, shaftLength, alongAxis);
    float spreadFade = 1.0 / (1.0 + 0.03 * alongAxis);
    float forwardScatter = 1.0 + 2.5 * pow(max(-alignment, 0.0), 6.0);
    float density = fogDensityAt(pointOnAxis.y) + clearAirDensity;
    float transmittance = exp(-fogOpticalDepth(cameraPosition, rayDirection, alongRay));

    vec3 color = beam.color * shaftStrength * core * lengthFade * spreadFade * forwardScatter * density * transmittance;

    vec3 toLamp = beam.position - cameraPosition;
    float lampDistance = length(toLamp);
    vec3 lampDirection = toLamp / lampDistance;
    float insideBeam = beamCone(-lampDirection);
    float glare = pow(max(dot(rayDirection, lampDirection), 0.0), 4000.0) * 6.0 + pow(max(dot(rayDirection, lampDirection), 0.0), 300.0) * 0.8;
    color += beam.color * glare * insideBeam * exp(-fogOpticalDepth(cameraPosition, lampDirection, lampDistance));

    fColor = vec4(color, 1.0);
}
