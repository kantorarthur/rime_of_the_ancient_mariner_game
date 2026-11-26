#version 410 core

layout(location = 0) in vec3 vOffset;

out vec3 fWorldPosition;
out float fAlong;
out float fAcross;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPosition;
uniform vec3 boxSize;
uniform vec3 displacement;
uniform vec3 velocity;
uniform float streakLength;
uniform float streakWidth;

void main()
{
    vec3 local = mod(vOffset * boxSize + displacement - cameraPosition, boxSize) - boxSize * 0.5;
    vec3 head = cameraPosition + local;
    vec3 direction = normalize(velocity);
    vec3 side = normalize(cross(direction, head - cameraPosition));

    float along = gl_VertexID < 2 ? 0.0 : 1.0;
    float across = gl_VertexID % 2 == 0 ? -1.0 : 1.0;
    vec3 position = head - direction * streakLength * along + side * streakWidth * 0.5 * across;

    fWorldPosition = position;
    fAlong = along;
    fAcross = across;
    gl_Position = projection * view * vec4(position, 1.0);
}
