#ifndef Fog_hpp
#define Fog_hpp

#include "graphics/Shader.hpp"

#include <glm/glm.hpp>

namespace gps {

    struct FogSettings
    {
        glm::vec3 color;
        float density;
        float heightFalloff;
        float glowStrength;
    };

    class Fog
    {
    public:
        void update(float deltaTime, float stormFactor);
        void toggle();
        bool isEnabled();
        void upload(gps::Shader shader);

    private:
        bool enabled = true;
        float amount = 1.0f;
        FogSettings current = {};
    };
}

#endif
