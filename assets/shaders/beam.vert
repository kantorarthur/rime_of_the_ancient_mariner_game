#version 410 core

layout(location = 0) in vec3 vPosition;

out vec3 fWorldPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPosition = model * vec4(vPosition, 1.0);
    fWorldPosition = worldPosition.xyz;
    gl_Position = projection * view * worldPosition;
}
