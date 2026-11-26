#version 410 core

#define MAX_POINT_LIGHTS 8

in vec3 fWorldPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

struct DirectionalLight
{
    vec3 direction;
    vec3 color;
    vec3 ambient;
};

struct PointLight
{
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec3 color;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
};

uniform DirectionalLight moon;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightCount;
uniform SpotLight beam;

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

float attenuation(float distance, float constant, float linear, float quadratic)
{
    return 1.0 / (constant + linear * distance + quadratic * distance * distance);
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
    color += shade(normalize(moon.direction), moon.color, normal, viewDirection, albedo, specularColor);

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
    float theta = dot(beamDirection, normalize(-beam.direction));
    float cone = clamp((theta - beam.outerCutOff) / (beam.cutOff - beam.outerCutOff), 0.0, 1.0);
    float beamFalloff = attenuation(beamDistance, beam.constant, beam.linear, beam.quadratic);
    color += cone * beamFalloff * shade(beamDirection, beam.color, normal, viewDirection, albedo, specularColor);

    color += texture(emissiveTexture, fTexCoords).rgb * emissiveStrength;

    fColor = vec4(color, 1.0);
}
