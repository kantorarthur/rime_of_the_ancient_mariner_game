#version 410 core

layout(location = 0) in vec3 vPosition;

out vec3 fDirection;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    fDirection = vPosition;
    vec4 position = projection * mat4(mat3(view)) * vec4(vPosition, 1.0);
    gl_Position = position.xyww;
}
