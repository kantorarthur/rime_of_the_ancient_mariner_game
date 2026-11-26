#include "game/ShipController.hpp"

#include <cmath>

namespace gps {

    const float MAX_SPEED = 9.0f;
    const float ACCELERATION = 0.8f;
    const float SAIL_CHANGE_SPEED = 0.35f;
    const float TURN_RATE = 14.0f;
    const float HULL_POINTS[3] = {-11.0f, 0.0f, 11.0f};
    const float HULL_RADIUS = 4.2f;

    void ShipController::reset(glm::vec2 position, float headingDegrees)
    {
        this->position = position;
        heading = headingDegrees;
        speed = 0.0f;
    }

    glm::vec2 ShipController::forward()
    {
        float angle = glm::radians(heading);
        return glm::vec2(-std::sin(angle), -std::cos(angle));
    }

    void ShipController::update(float deltaTime, float sailInput, float helmInput, float windStrength, std::vector<Obstacle>& obstacles)
    {
        sail = glm::clamp(sail + sailInput * SAIL_CHANGE_SPEED * deltaTime, 0.0f, 1.0f);

        float targetSpeed = sail * MAX_SPEED * windStrength;
        float step = ACCELERATION * deltaTime;
        speed += glm::clamp(targetSpeed - speed, -step, step);

        float steering = glm::clamp(std::abs(speed) / 4.0f, 0.35f, 1.0f);
        heading += helmInput * TURN_RATE * steering * deltaTime;
        if(heading >= 360.0f)
            heading -= 360.0f;
        if(heading < 0.0f)
            heading += 360.0f;

        position += forward() * speed * deltaTime;
        resolveCollisions(obstacles);
    }

    void ShipController::resolveCollisions(std::vector<Obstacle>& obstacles)
    {
        collided = false;

        for(float offset : HULL_POINTS)
        {
            for(Obstacle& obstacle : obstacles)
            {
                glm::vec2 hullPoint = position + forward() * offset;
                glm::vec2 away = hullPoint - obstacle.center;
                float distance = glm::length(away);
                float overlap = HULL_RADIUS + obstacle.radius - distance;
                if(overlap <= 0.0f || distance < 0.0001f)
                    continue;

                position += away / distance * overlap;
                collided = true;
            }
        }

        if(collided)
            speed *= 0.3f;
    }

    glm::vec2 ShipController::getPosition()
    {
        return position;
    }

    float ShipController::getHeadingDegrees()
    {
        return heading;
    }

    float ShipController::getSpeed()
    {
        return speed;
    }

    float ShipController::getSail()
    {
        return sail;
    }

    bool ShipController::hasCollided()
    {
        return collided;
    }
}
