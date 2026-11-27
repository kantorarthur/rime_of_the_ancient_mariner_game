#version 410 core

in float fAlpha;

out vec4 fColor;

const vec3 SOUL_COLOR = vec3(0.75, 0.9, 1.0);

void main()
{
    float distance = length(gl_PointCoord - vec2(0.5)) * 2.0;
    float glow = 0.35 * exp(-distance * distance * 4.0) + exp(-distance * distance * 30.0);
    fColor = vec4(SOUL_COLOR * glow * fAlpha * 2.2, 1.0);
}
