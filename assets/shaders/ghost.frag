#version 410 core

#include "lights.glsl"
#include "fog.glsl"

in vec3 fWorldPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

uniform vec3 cameraPosition;
uniform float ghostFade;
uniform sampler2D diffuseTexture;

const vec3 GHOST_COLOR = vec3(0.55, 0.72, 0.85);

void main()
{
    vec4 albedo = texture(diffuseTexture, fTexCoords);
    if(albedo.a < 0.5)
        discard;

    vec3 normal = normalize(fNormal);
    vec3 toCamera = cameraPosition - fWorldPosition;
    float distance = length(toCamera);
    float facing = abs(dot(normal, toCamera / distance));
    float rim = pow(1.0 - facing, 2.0);
    float detail = dot(albedo.rgb, vec3(0.3, 0.59, 0.11));

    vec3 color = GHOST_COLOR * (0.05 + 0.45 * rim) * (0.4 + 1.4 * detail);
    float transmittance = exp(-fogOpticalDepth(cameraPosition, -toCamera / distance, distance));
    fColor = vec4(color * ghostFade * mix(0.3, 1.0, transmittance), 1.0);
}
