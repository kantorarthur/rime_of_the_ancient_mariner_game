#include "scene/Wind.hpp"

#include <cmath>

namespace gps {

    const float WIND_BASE_ANGLE = 37.0f;
    const float WIND_ANGLE_SWING = 20.0f;
    const float CALM_WIND_SPEED = 0.3f;
    const float STORM_WIND_SPEED = 11.0f;

    void Wind::update(float deltaTime, float stormFactor)
    {
        time += deltaTime;

        float angle = glm::radians(WIND_BASE_ANGLE + WIND_ANGLE_SWING * std::sin(time * 0.07f));
        float gust = 0.7f + 0.2f * std::sin(time * 0.9f) + 0.15f * std::sin(time * 2.3f + 1.7f) + 0.1f * std::sin(time * 5.1f);
        float speed = glm::mix(CALM_WIND_SPEED, STORM_WIND_SPEED, stormFactor) * gust;

        velocity = glm::vec3(std::cos(angle), 0.0f, std::sin(angle)) * speed;
    }

    glm::vec3 Wind::getVelocity()
    {
        return velocity;
    }
}
