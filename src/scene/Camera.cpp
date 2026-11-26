#include "scene/Camera.hpp"

#include <cmath>

namespace gps {

    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp)
    {
        this->cameraPosition = cameraPosition;
        this->worldUpDirection = glm::normalize(cameraUp);

        glm::vec3 front = glm::normalize(cameraTarget - cameraPosition);
        this->pitch = glm::degrees(std::asin(front.y));
        this->yaw = glm::degrees(std::atan2(front.z, front.x));

        updateVectors();
    }

    glm::mat4 Camera::getViewMatrix()
    {
        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
    }

    void Camera::move(MOVE_DIRECTION direction, float speed)
    {
        switch(direction)
        {
        case MOVE_FORWARD:
            cameraPosition += cameraFrontDirection * speed;
            break;
        case MOVE_BACKWARD:
            cameraPosition -= cameraFrontDirection * speed;
            break;
        case MOVE_RIGHT:
            cameraPosition += cameraRightDirection * speed;
            break;
        case MOVE_LEFT:
            cameraPosition -= cameraRightDirection * speed;
            break;
        case MOVE_UP:
            cameraPosition += worldUpDirection * speed;
            break;
        case MOVE_DOWN:
            cameraPosition -= worldUpDirection * speed;
            break;
        }

        cameraTarget = cameraPosition + cameraFrontDirection;
    }

    void Camera::rotate(float pitch, float yaw)
    {
        this->pitch += pitch;
        this->yaw += yaw;

        if(this->pitch > 89.0f)
            this->pitch = 89.0f;
        if(this->pitch < -89.0f)
            this->pitch = -89.0f;

        updateVectors();
    }

    glm::vec3 Camera::getPosition()
    {
        return cameraPosition;
    }

    glm::vec3 Camera::getFrontDirection()
    {
        return cameraFrontDirection;
    }

    void Camera::setPosition(glm::vec3 position)
    {
        cameraPosition = position;
        cameraTarget = cameraPosition + cameraFrontDirection;
    }

    void Camera::updateVectors()
    {
        float yawRad = glm::radians(yaw);
        float pitchRad = glm::radians(pitch);

        glm::vec3 front;
        front.x = std::cos(yawRad) * std::cos(pitchRad);
        front.y = std::sin(pitchRad);
        front.z = std::sin(yawRad) * std::cos(pitchRad);

        cameraFrontDirection = glm::normalize(front);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, worldUpDirection));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
        cameraTarget = cameraPosition + cameraFrontDirection;
    }
}
