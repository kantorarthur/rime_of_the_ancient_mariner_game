#ifndef Lightning_hpp
#define Lightning_hpp

#include "core/OpenGL.hpp"
#include "graphics/Shader.hpp"

#include <glm/glm.hpp>

#include <random>
#include <vector>

namespace gps {

    struct BoltSegment
    {
        glm::vec3 start;
        glm::vec3 end;
        float width;
    };

    struct FlashPulse
    {
        float start;
        float strength;
    };

    class Lightning
    {
    public:
        Lightning() = default;
        ~Lightning();
        Lightning(const Lightning&) = delete;
        Lightning& operator=(const Lightning&) = delete;

        void init();
        void update(float deltaTime, float stormFactor, glm::vec3 cameraPosition, glm::vec3 viewDirection);
        void strike(glm::vec3 cameraPosition, glm::vec3 viewDirection);
        void draw(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition);

        float getFlash();
        glm::vec3 getDirection();

    private:
        GLuint vao = 0;
        GLuint vbo = 0;

        std::mt19937 random = std::mt19937(2024);
        std::vector<BoltSegment> segments;
        std::vector<FlashPulse> pulses;

        float sinceStrike = 100.0f;
        float untilNextStrike = 4.0f;
        float flash = 0.0f;
        glm::vec3 direction = glm::vec3(0.0f, 1.0f, 0.0f);

        float randomRange(float low, float high);
        void addBolt(glm::vec3 start, glm::vec3 end, float width, int depth, bool canBranch);
    };
}

#endif
