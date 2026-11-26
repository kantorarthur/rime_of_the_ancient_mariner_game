#version 410 core

#define WAVE_COUNT 6

layout(location = 0) in vec2 vPosition;

struct Wave
{
    vec2 direction;
    float wavelength;
    float amplitude;
    float steepness;
};

uniform Wave waves[WAVE_COUNT];
uniform mat4 view;
uniform mat4 projection;
uniform vec2 gridCenter;
uniform vec3 cameraPosition;
uniform float time;
uniform vec3 shelter;

out vec3 fWorldPosition;
out vec3 fNormal;
out float fCrest;
out float fDistance;

const float PI = 3.14159265;
const float GRAVITY = 9.81;

void main()
{
    vec2 xz = vPosition + gridCenter;
    float distance = length(xz - cameraPosition.xz);
    float fade = 1.0 - smoothstep(150.0, 800.0, distance);
    float sheltered = mix(shelter.z, 1.0, smoothstep(shelter.x, shelter.y, length(xz)));
    fade *= sheltered;

    vec3 position = vec3(xz.x, 0.0, xz.y);
    vec3 normal = vec3(0.0, 1.0, 0.0);
    float crest = 0.0;

    for(int i = 0; i < WAVE_COUNT; i++)
    {
        float k = 2.0 * PI / waves[i].wavelength;
        float speed = sqrt(GRAVITY / k);
        vec2 direction = normalize(waves[i].direction);
        float phase = k * (dot(direction, xz) - speed * time);
        float amplitude = waves[i].amplitude * fade;
        float steepness = waves[i].steepness;

        position.x += steepness * amplitude * direction.x * cos(phase);
        position.z += steepness * amplitude * direction.y * cos(phase);
        position.y += amplitude * sin(phase);

        normal.x -= direction.x * k * amplitude * cos(phase);
        normal.z -= direction.y * k * amplitude * cos(phase);
        normal.y -= steepness * k * amplitude * sin(phase);

        crest += steepness * k * amplitude * sin(phase);
    }

    fWorldPosition = position;
    fNormal = normal;
    fCrest = crest;
    fDistance = distance;
    gl_Position = projection * view * vec4(position, 1.0);
}
