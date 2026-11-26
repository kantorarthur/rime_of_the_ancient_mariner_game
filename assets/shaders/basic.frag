#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

uniform bool flatShading;

vec3 ambient;
vec3 diffuse;
vec3 specular;

float ambientStrength = 0.2f;
float specularStrength = 0.5f;
float shininess = 32.0f;

void computeDirLight()
{
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);

    vec3 normalEye;
    if(flatShading)
        normalEye = normalize(cross(dFdx(fPosEye.xyz), dFdy(fPosEye.xyz)));
    else
        normalEye = normalize(normalMatrix * fNormal);

    vec3 lightDirN = normalize(vec3(view * vec4(lightDir, 0.0f)));
    vec3 viewDir = normalize(-fPosEye.xyz);

    ambient = ambientStrength * lightColor;
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
    specular = specularStrength * specCoeff * lightColor;
}

void main()
{
    computeDirLight();

    vec3 baseColor = texture(diffuseTexture, fTexCoords).rgb;
    vec3 specColor = texture(specularTexture, fTexCoords).rgb;
    vec3 color = min((ambient + diffuse) * baseColor + specular * specColor, 1.0f);

    fColor = vec4(color, 1.0f);
}
