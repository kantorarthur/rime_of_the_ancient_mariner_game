#include "graphics/Animator.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace gps {

    void Animator::attach(gps::SkinnedMesh* mesh)
    {
        this->mesh = mesh;
        globals.assign(mesh->getBones().size(), glm::mat4(1.0f));
        palette.assign(mesh->getBones().size(), glm::mat4(1.0f));
    }

    std::vector<int> Animator::mapBones(gps::AnimationClip* source)
    {
        std::vector<int> mapping(mesh->getBones().size(), -1);
        for(int i = 0; i < source->getBoneCount(); i++)
        {
            int bone = mesh->findBone(source->getBoneName(i));
            if(bone >= 0)
                mapping[bone] = i;
        }
        return mapping;
    }

    void Animator::play(gps::AnimationClip* newClip, bool newLoop, float blendTime)
    {
        if(newClip == clip)
            return;

        previousClip = clip;
        previousBones = clipBones;
        previousTime = time;
        previousLoop = loop;

        clip = newClip;
        clipBones = mapBones(newClip);
        time = 0.0f;
        loop = newLoop;

        blendDuration = previousClip ? blendTime : 0.0f;
        blend = blendDuration > 0.0f ? 0.0f : 1.0f;
    }

    BonePose Animator::samplePose(gps::AnimationClip* source, std::vector<int>& mapping, int bone, float sampleTime, bool sampleLoop)
    {
        if(source == nullptr || mapping[bone] < 0)
            return {mesh->getBones()[bone].restRotation, glm::vec3(0.0f)};
        return source->sample(mapping[bone], sampleTime, sampleLoop);
    }

    void Animator::update(float deltaTime)
    {
        if(mesh == nullptr)
            return;

        time += deltaTime;
        previousTime += deltaTime;
        if(blend < 1.0f)
            blend = glm::min(1.0f, blend + deltaTime / blendDuration);

        std::vector<Bone>& bones = mesh->getBones();
        for(size_t i = 0; i < bones.size(); i++)
        {
            BonePose pose = samplePose(clip, clipBones, (int)i, time, loop);
            if(blend < 1.0f)
            {
                BonePose old = samplePose(previousClip, previousBones, (int)i, previousTime, previousLoop);
                pose.rotation = glm::slerp(old.rotation, pose.rotation, blend);
                pose.offset = glm::mix(old.offset, pose.offset, blend);
            }

            glm::mat4 local = glm::translate(glm::mat4(1.0f), bones[i].restTranslation + pose.offset) * glm::mat4_cast(pose.rotation);
            if(bones[i].parent >= 0)
                globals[i] = globals[bones[i].parent] * local;
            else
                globals[i] = local;
            palette[i] = globals[i] * bones[i].inverseBind;
        }
    }

    std::vector<glm::mat4>& Animator::getPalette()
    {
        return palette;
    }

    bool Animator::isFinished()
    {
        return clip != nullptr && !loop && time >= clip->getDuration();
    }

    gps::AnimationClip* Animator::getClip()
    {
        return clip;
    }

    glm::mat4 Animator::getBoneTransform(int bone)
    {
        return globals[bone];
    }
}
