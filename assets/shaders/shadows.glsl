uniform sampler2DShadow moonShadowMap;
uniform sampler2DShadow beamShadowMap;
uniform mat4 moonLightSpace;
uniform mat4 beamLightSpace;
uniform vec2 shadowTexelSize;

float sampleShadow(sampler2DShadow shadowMap, mat4 lightSpace, vec3 worldPosition, float texelSize)
{
    vec4 lightPosition = lightSpace * vec4(worldPosition, 1.0);
    if(lightPosition.w <= 0.0)
        return 1.0;

    vec3 coords = lightPosition.xyz / lightPosition.w * 0.5 + 0.5;
    if(coords.z > 1.0 || coords.x < 0.0 || coords.x > 1.0 || coords.y < 0.0 || coords.y > 1.0)
        return 1.0;

    float lit = 0.0;
    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
            lit += texture(shadowMap, vec3(coords.xy + vec2(x, y) * texelSize, coords.z));
    }
    return lit / 9.0;
}

float moonShadow(vec3 worldPosition, vec3 normal)
{
    return sampleShadow(moonShadowMap, moonLightSpace, worldPosition + normal * 0.06, shadowTexelSize.x);
}

float beamShadow(vec3 worldPosition, vec3 normal)
{
    return sampleShadow(beamShadowMap, beamLightSpace, worldPosition + normal * 0.08, shadowTexelSize.y);
}
