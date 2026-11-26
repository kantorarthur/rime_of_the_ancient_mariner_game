#ifndef Character_hpp
#define Character_hpp

#include "graphics/Animator.hpp"
#include "graphics/Model3D.hpp"
#include "graphics/SkinnedMesh.hpp"

#include <glm/glm.hpp>

namespace gps {

    struct Character
    {
        gps::SkinnedMesh* mesh = nullptr;
        gps::Animator animator;
        glm::vec3 localPosition = glm::vec3(0.0f);
        float localYaw = 0.0f;
        float targetYaw = 0.0f;
        float scale = 1.0f;
        int parentObject = -1;
        bool visible = true;

        gps::Model3D* prop = nullptr;
        int propBone = -1;
        glm::mat4 propOffset = glm::mat4(1.0f);
        bool propVisible = false;
    };
}

#endif
