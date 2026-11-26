#define MAX_POINT_LIGHTS 8

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
uniform float flash;

float attenuation(float distance, float constant, float linear, float quadratic)
{
    return 1.0 / (constant + linear * distance + quadratic * distance * distance);
}

float beamCone(vec3 toBeamDirection)
{
    float theta = dot(toBeamDirection, normalize(-beam.direction));
    return clamp((theta - beam.outerCutOff) / (beam.cutOff - beam.outerCutOff), 0.0, 1.0);
}
