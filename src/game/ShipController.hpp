#ifndef ShipController_hpp
#define ShipController_hpp

#include <glm/glm.hpp>

#include <vector>

namespace gps {

    struct Obstacle
    {
        glm::vec2 center;
        float radius;
    };

    class ShipController
    {
    public:
        void reset(glm::vec2 position, float headingDegrees);
        void update(float deltaTime, float sailInput, float helmInput, float windStrength, std::vector<Obstacle>& obstacles);

        glm::vec2 getPosition();
        float getHeadingDegrees();
        float getSpeed();
        float getSail();
        bool hasCollided();

    private:
        glm::vec2 position = glm::vec2(0.0f);
        float heading = 0.0f;
        float speed = 0.0f;
        float sail = 0.4f;
        bool collided = false;

        glm::vec2 forward();
        void resolveCollisions(std::vector<Obstacle>& obstacles);
    };
}

#endif
