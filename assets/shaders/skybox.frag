#version 410 core

in vec3 fDirection;

out vec4 fColor;

uniform samplerCube skybox;
uniform float brightness;

void main()
{
    fColor = vec4(texture(skybox, fDirection).rgb * brightness, 1.0);
}
