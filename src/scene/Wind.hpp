#ifndef Wind_hpp
#define Wind_hpp

#include <glm/glm.hpp>

namespace gps {

    class Wind
    {
    public:
        void update(float deltaTime, float stormFactor);
        glm::vec3 getVelocity();

    private:
        float time = 0.0f;
        glm::vec3 velocity = glm::vec3(0.0f);
    };
}

#endif
