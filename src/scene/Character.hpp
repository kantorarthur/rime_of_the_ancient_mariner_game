#ifndef Character_hpp
#define Character_hpp

#include "graphics/Animator.hpp"
#include "graphics/Model3D.hpp"
#include "graphics/SkinnedMesh.hpp"

#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace gps {

    struct Prop
    {
        gps::Model3D* model = nullptr;
        int bone = -1;
        glm::mat4 offset = glm::mat4(1.0f);
        float wingFlap = 0.0f;
        bool visible = false;
    };

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
        std::string nextClip;
        std::vector<Prop> props;
    };
}

#endif
