#version 410 core

in vec2 fTexCoords;

uniform sampler2D diffuseTexture;

void main()
{
    if(texture(diffuseTexture, fTexCoords).a < 0.5)
        discard;
}
