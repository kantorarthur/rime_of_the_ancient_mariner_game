#version 410 core

layout(location = 0) in vec2 vPosition;

uniform vec2 screenSize;

void main()
{
    vec2 normalized = vPosition / screenSize;
    gl_Position = vec4(normalized.x * 2.0 - 1.0, 1.0 - normalized.y * 2.0, 0.0, 1.0);
}
