#include "graphics/AnimationClip.hpp"

#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>

namespace gps {

    bool AnimationClip::load(std::string fileName)
    {
        std::ifstream file(fileName, std::ios::binary);
        char magic[4];
        file.read(magic, 4);
        if(!file || std::strncmp(magic, "ANIM", 4) != 0)
        {
            std::cerr << "ERROR: could not load animation " << fileName << std::endl;
            return false;
        }

        uint32_t version;
        uint32_t frames;
        uint32_t boneCount;
        file.read((char*)&version, sizeof(uint32_t));
        file.read((char*)&framesPerSecond, sizeof(float));
        file.read((char*)&frames, sizeof(uint32_t));
        file.read((char*)&boneCount, sizeof(uint32_t));
        frameCount = (int)frames;

        for(uint32_t i = 0; i < boneCount; i++)
        {
            char name[33] = {};
            file.read(name, 32);
            boneNames.push_back(name);
        }

        poses.resize(frames * boneCount);
        for(BonePose& pose : poses)
        {
            float values[7];
            file.read((char*)values, 7 * sizeof(float));
            pose.rotation = glm::quat(values[3], values[0], values[1], values[2]);
            pose.offset = glm::vec3(values[4], values[5], values[6]);
        }
        return true;
    }

    float AnimationClip::getDuration()
    {
        return (frameCount - 1) / framesPerSecond;
    }

    int AnimationClip::getBoneCount()
    {
        return (int)boneNames.size();
    }

    std::string AnimationClip::getBoneName(int bone)
    {
        return boneNames[bone];
    }

    BonePose AnimationClip::sample(int bone, float time, bool loop)
    {
        float duration = getDuration();
        if(loop && duration > 0.0f)
            time = std::fmod(time, duration);
        time = glm::clamp(time, 0.0f, duration);

        float frame = time * framesPerSecond;
        int first = (int)frame;
        int second = glm::min(first + 1, frameCount - 1);
        float blend = frame - first;

        BonePose& a = poses[first * boneNames.size() + bone];
        BonePose& b = poses[second * boneNames.size() + bone];

        BonePose result;
        result.rotation = glm::slerp(a.rotation, b.rotation, blend);
        result.offset = glm::mix(a.offset, b.offset, blend);
        return result;
    }
}
