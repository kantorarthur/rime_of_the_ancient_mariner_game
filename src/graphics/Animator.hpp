#ifndef Animator_hpp
#define Animator_hpp

#include "graphics/AnimationClip.hpp"
#include "graphics/SkinnedMesh.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace gps {

    class Animator
    {
    public:
        void attach(gps::SkinnedMesh* mesh);
        void play(gps::AnimationClip* clip, bool loop, float blendTime);
        void update(float deltaTime);

        std::vector<glm::mat4>& getPalette();
        bool isFinished();
        gps::AnimationClip* getClip();
        glm::mat4 getBoneTransform(int bone);

    private:
        gps::SkinnedMesh* mesh = nullptr;

        gps::AnimationClip* clip = nullptr;
        std::vector<int> clipBones;
        float time = 0.0f;
        bool loop = true;

        gps::AnimationClip* previousClip = nullptr;
        std::vector<int> previousBones;
        float previousTime = 0.0f;
        bool previousLoop = true;
        float blend = 1.0f;
        float blendDuration = 0.0f;

        std::vector<glm::mat4> globals;
        std::vector<glm::mat4> palette;

        std::vector<int> mapBones(gps::AnimationClip* source);
        BonePose samplePose(gps::AnimationClip* source, std::vector<int>& mapping, int bone, float sampleTime, bool sampleLoop);
    };
}

#endif
