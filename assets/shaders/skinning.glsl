#define MAX_SKIN_BONES 32

layout(location = 3) in uvec4 vBoneIds;
layout(location = 4) in vec4 vBoneWeights;

uniform mat4 bones[MAX_SKIN_BONES];

mat4 skinMatrix()
{
    return bones[vBoneIds.x] * vBoneWeights.x
         + bones[vBoneIds.y] * vBoneWeights.y
         + bones[vBoneIds.z] * vBoneWeights.z
         + bones[vBoneIds.w] * vBoneWeights.w;
}
