struct Fog
{
    vec3 color;
    float density;
    float heightFalloff;
    float glowStrength;
};

uniform Fog fog;

float fogDensityAt(float height)
{
    return fog.density * exp(-fog.heightFalloff * max(height, -2.0));
}

float fogOpticalDepth(vec3 eye, vec3 direction, float distance)
{
    float startDensity = fogDensityAt(eye.y);
    float slope = fog.heightFalloff * direction.y;
    if(abs(slope) < 0.00001)
        return startDensity * distance;
    float exponent = clamp(-slope * distance, -60.0, 60.0);
    return startDensity * (1.0 - exp(exponent)) / slope;
}

vec3 fogScatterColor(vec3 direction)
{
    float towardMoon = max(dot(direction, normalize(moon.direction)), 0.0);
    return fog.color + moon.color * (0.05 * pow(towardMoon, 6.0) + 0.25 * pow(towardMoon, 64.0));
}

vec3 fogLightGlow(vec3 eye, vec3 direction, float distance)
{
    vec3 glow = vec3(0.0);
    for(int i = 0; i < pointLightCount; i++)
    {
        vec3 toLight = pointLights[i].position - eye;
        float along = dot(toLight, direction);
        float closest = max(length(toLight - direction * along), 0.2);
        float integral = (atan((distance - along) / closest) + atan(along / closest)) / closest;
        float reach = exp(-closest * 0.12);
        float behindSurface = mix(0.3, 1.0, smoothstep(-3.0, 0.0, distance - along));
        glow += pointLights[i].color * integral * reach * behindSurface * fogDensityAt(pointLights[i].position.y);
    }
    return glow * fog.glowStrength;
}

vec3 applyFogAlongRay(vec3 color, vec3 eye, vec3 direction, float distance)
{
    float transmittance = exp(-fogOpticalDepth(eye, direction, distance));
    vec3 scattered = fogScatterColor(direction) * (1.0 - transmittance);
    return color * transmittance + scattered + fogLightGlow(eye, direction, distance);
}

vec3 applyFog(vec3 color, vec3 eye, vec3 worldPosition)
{
    vec3 toPoint = worldPosition - eye;
    float distance = max(length(toPoint), 0.0001);
    return applyFogAlongRay(color, eye, toPoint / distance, distance);
}
