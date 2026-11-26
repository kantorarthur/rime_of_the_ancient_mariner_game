#version 410 core

#define MAX_FOAM_CIRCLES 12

#include "lights.glsl"
#include "fog.glsl"

in vec3 fWorldPosition;
in vec3 fNormal;
in float fCrest;
in float fDistance;

out vec4 fColor;

uniform samplerCube skybox;
uniform vec3 cameraPosition;
uniform float time;
uniform float stormFactor;
uniform vec4 foamCircles[MAX_FOAM_CIRCLES];
uniform int foamCircleCount;

const vec3 DEEP_COLOR = vec3(0.002, 0.008, 0.014);
const vec3 FOAM_COLOR = vec3(0.75, 0.8, 0.85);

float hash(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float valueNoise(vec2 p)
{
    vec2 cell = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    float a = hash(cell);
    float b = hash(cell + vec2(1.0, 0.0));
    float c = hash(cell + vec2(0.0, 1.0));
    float d = hash(cell + vec2(1.0, 1.0));
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

vec2 rippleSlope(vec2 p)
{
    vec2 slope = vec2(0.0);
    vec2 directions[4] = vec2[4](vec2(0.9, 0.4), vec2(-0.3, 0.95), vec2(0.6, -0.8), vec2(-0.85, -0.5));
    float frequencies[4] = float[4](1.7, 2.9, 4.3, 6.1);
    float speeds[4] = float[4](1.3, 1.7, 2.1, 2.6);
    for(int i = 0; i < 4; i++)
    {
        float phase = dot(directions[i], p) * frequencies[i] + time * speeds[i];
        slope += directions[i] * cos(phase) * frequencies[i] * (0.012 / float(i + 1));
    }
    return slope;
}

void main()
{
    vec3 viewDirection = normalize(cameraPosition - fWorldPosition);
    float detail = (1.0 - smoothstep(20.0, 160.0, fDistance)) * mix(0.7, 1.6, stormFactor);
    vec2 slope = rippleSlope(fWorldPosition.xz) * detail;
    vec3 normal = normalize(normalize(fNormal) + vec3(-slope.x, 0.0, -slope.y));
    if(dot(normal, viewDirection) < 0.0)
        normal = normalize(normal + viewDirection * (0.01 - dot(normal, viewDirection)));

    float facing = max(dot(normal, viewDirection), 0.0);
    float fresnel = 0.02 + 0.98 * pow(1.0 - facing, 5.0);

    vec3 reflected = reflect(-viewDirection, normal);
    reflected.y = max(reflected.y, 0.02);
    vec3 reflection = texture(skybox, reflected).rgb * mix(0.85, 0.5, stormFactor);

    vec3 toMoon = normalize(moon.direction);
    vec3 body = DEEP_COLOR + moon.ambient * 0.25 + moon.color * 0.02 * max(dot(normal, toMoon), 0.0);
    vec3 color = mix(body, reflection, fresnel);

    vec3 halfway = normalize(toMoon + viewDirection);
    color += moon.color * pow(max(dot(normal, halfway), 0.0), mix(1400.0, 350.0, stormFactor)) * mix(5.0, 2.0, stormFactor);

    for(int i = 0; i < pointLightCount; i++)
    {
        vec3 toLight = pointLights[i].position - fWorldPosition;
        float distance = length(toLight);
        vec3 lightDirection = toLight / distance;
        float falloff = attenuation(distance * 0.35, pointLights[i].constant, pointLights[i].linear, pointLights[i].quadratic);
        vec3 lightHalfway = normalize(lightDirection + viewDirection);
        color += pointLights[i].color * falloff * pow(max(dot(normal, lightHalfway), 0.0), mix(400.0, 120.0, stormFactor)) * 2.5;
    }

    vec3 toBeam = beam.position - fWorldPosition;
    float beamDistance = length(toBeam);
    vec3 beamDirection = toBeam / beamDistance;
    float cone = beamCone(beamDirection);
    float beamFalloff = attenuation(beamDistance, beam.constant, beam.linear, beam.quadratic);
    vec3 beamHalfway = normalize(beamDirection + viewDirection);
    color += cone * beamFalloff * beam.color * (0.04 + 1.5 * pow(max(dot(normal, beamHalfway), 0.0), 80.0));

    float foam = smoothstep(0.35, 0.75, fCrest) * stormFactor;
    for(int i = 0; i < foamCircleCount; i++)
    {
        float edge = length(fWorldPosition.xz - foamCircles[i].xy) - foamCircles[i].z;
        float band = 1.0 - smoothstep(0.0, foamCircles[i].w * mix(1.0, 1.8, stormFactor), abs(edge));
        foam += band * mix(0.55, 0.9, stormFactor);
    }
    float pattern = valueNoise(fWorldPosition.xz * 0.9 + vec2(time * 0.25, time * 0.1));
    pattern *= valueNoise(fWorldPosition.xz * 2.7 - vec2(time * 0.15, -time * 0.2));
    foam = clamp(foam * smoothstep(0.12, 0.45, pattern) * 1.4, 0.0, 1.0);
    foam *= 1.0 - smoothstep(60.0, 200.0, fDistance);

    vec3 foamLight = moon.ambient * 4.0 + moon.color * max(dot(normal, toMoon), 0.2) + cone * beamFalloff * beam.color * 0.6;
    color = mix(color, FOAM_COLOR * foamLight, foam);

    vec3 horizonDirection = normalize(vec3(-viewDirection.x, 0.0, -viewDirection.z));
    vec3 horizon = texture(skybox, horizonDirection).rgb;
    color = mix(color, horizon, smoothstep(180.0, 1100.0, fDistance));

    fColor = vec4(applyFog(color, cameraPosition, fWorldPosition), 1.0);
}
