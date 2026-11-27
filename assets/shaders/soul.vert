#version 410 core

layout(location = 0) in vec4 vSoul;

out float fAlpha;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 viewPosition = view * vec4(vSoul.xyz, 1.0);
    fAlpha = vSoul.w;
    gl_Position = projection * viewPosition;
    gl_PointSize = clamp(900.0 / max(-viewPosition.z, 0.1), 6.0, 256.0);
}
