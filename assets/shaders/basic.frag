#version 410 core

#include "lights.glsl"
#include "fog.glsl"
#include "shadows.glsl"

in vec3 fWorldPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

uniform vec3 cameraPosition;
uniform float shininess;
uniform float emissiveStrength;
uniform bool flatShading;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D emissiveTexture;

vec3 shade(vec3 lightDirection, vec3 lightColor, vec3 normal, vec3 viewDirection, vec3 albedo, vec3 specularColor)
{
    float diffuse = max(dot(normal, lightDirection), 0.0);
    vec3 halfway = normalize(lightDirection + viewDirection);
    float specular = 0.0;
    if(diffuse > 0.0)
        specular = pow(max(dot(normal, halfway), 0.0), shininess);
    return lightColor * (diffuse * albedo + specular * specularColor);
}

void main()
{
    vec4 albedoSample = texture(diffuseTexture, fTexCoords);
    if(albedoSample.a < 0.5)
        discard;

    vec3 albedo = albedoSample.rgb;
    vec3 specularColor = texture(specularTexture, fTexCoords).rgb;
    vec3 viewDirection = normalize(cameraPosition - fWorldPosition);

    vec3 normal;
    if(flatShading)
        normal = normalize(cross(dFdx(fWorldPosition), dFdy(fWorldPosition)));
    else
        normal = normalize(fNormal);
    if(dot(normal, viewDirection) < 0.0)
        normal = -normal;

    vec3 color = moon.ambient * albedo;
    color += moonShadow(fWorldPosition, normal) * shade(normalize(moon.direction), moon.color, normal, viewDirection, albedo, specularColor);

    for(int i = 0; i < pointLightCount; i++)
    {
        vec3 toLight = pointLights[i].position - fWorldPosition;
        float distance = length(toLight);
        float falloff = attenuation(distance, pointLights[i].constant, pointLights[i].linear, pointLights[i].quadratic);
        color += falloff * shade(toLight / distance, pointLights[i].color, normal, viewDirection, albedo, specularColor);
    }

    vec3 toBeam = beam.position - fWorldPosition;
    float beamDistance = length(toBeam);
    vec3 beamDirection = toBeam / beamDistance;
    float beamFalloff = attenuation(beamDistance, beam.constant, beam.linear, beam.quadratic);
    color += beamCone(beamDirection) * beamFalloff * beamShadow(fWorldPosition, normal) * shade(beamDirection, beam.color, normal, viewDirection, albedo, specularColor);

    color += texture(emissiveTexture, fTexCoords).rgb * emissiveStrength;

    fColor = vec4(applyFog(color, cameraPosition, fWorldPosition), 1.0);
}
