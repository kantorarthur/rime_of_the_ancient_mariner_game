#ifndef AnimationClip_hpp
#define AnimationClip_hpp

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <vector>

namespace gps {

    struct BonePose
    {
        glm::quat rotation;
        glm::vec3 offset;
    };

    class AnimationClip
    {
    public:
        bool load(std::string fileName);

        float getDuration();
        int getBoneCount();
        std::string getBoneName(int bone);
        BonePose sample(int bone, float time, bool loop);

    private:
        float framesPerSecond = 24.0f;
        int frameCount = 0;
        std::vector<std::string> boneNames;
        std::vector<BonePose> poses;
    };
}

#endif
