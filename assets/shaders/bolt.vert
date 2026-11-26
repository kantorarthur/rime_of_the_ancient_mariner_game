#version 410 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in float vAcross;

out vec3 fWorldPosition;
out float fAcross;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    fWorldPosition = vPosition;
    fAcross = vAcross;
    gl_Position = projection * view * vec4(vPosition, 1.0);
}
