#include "game/Game.hpp"

#include <cmath>
#include <cstdio>

namespace gps {

    const float FREE_CAMERA_SPEED = 8.0f;
    const float SPRINT_MULTIPLIER = 3.0f;
    const float MOUSE_SENSITIVITY = 0.1f;
    const float PROLOGUE_ORBIT_SPEED = 5.0f;
    const float CHASE_TARGET_HEIGHT = 9.0f;
    const float CHASE_SMOOTHING = 4.0f;
    const glm::vec4 HINT_COLOR = glm::vec4(0.8f, 0.8f, 0.8f, 0.85f);

    void Game::start(gps::Scene& scene)
    {
        story.start();
        lastChapter = story.getChapter();
        scene.setWeather(Weather::Calm);
    }

    void Game::update(float deltaTime, gps::Input& input, gps::Camera& camera, gps::Scene& scene)
    {
        if(input.wasKeyPressed(GLFW_KEY_C))
        {
            if(mode == ControlMode::Ship)
                mode = ControlMode::FreeCamera;
            else
            {
                mode = ControlMode::Ship;
                chaseStarted = false;
            }
        }

        story.update(deltaTime, glm::length(glm::vec2(scene.getShipPosition().x, scene.getShipPosition().z)));
        reactToStory(scene);

        if(mode == ControlMode::Ship)
        {
            updateShip(input, scene);
            updateChaseCamera(deltaTime, input, camera, scene);
        }
        else
        {
            scene.setShipControls(0.0f, 0.0f);
            updateFreeCamera(deltaTime, input, camera);
        }
    }

    void Game::reactToStory(gps::Scene& scene)
    {
        Chapter chapter = story.getChapter();
        if(chapter == lastChapter)
            return;

        if(chapter == Chapter::Storm)
            scene.setWeather(Weather::Storm);

        lastChapter = chapter;
    }

    void Game::updateShip(gps::Input& input, gps::Scene& scene)
    {
        float sail = 0.0f;
        float helm = 0.0f;

        if(input.isKeyDown(GLFW_KEY_W))
            sail += 1.0f;
        if(input.isKeyDown(GLFW_KEY_S))
            sail -= 1.0f;
        if(input.isKeyDown(GLFW_KEY_A))
            helm += 1.0f;
        if(input.isKeyDown(GLFW_KEY_D))
            helm -= 1.0f;

        scene.setShipControls(sail, helm);
    }

    void Game::updateChaseCamera(float deltaTime, gps::Input& input, gps::Camera& camera, gps::Scene& scene)
    {
        glm::vec2 mouseDelta = input.getMouseDelta() * MOUSE_SENSITIVITY;
        if(mouseDelta != glm::vec2(0.0f))
            playerMovedCamera = true;

        if(story.getChapter() == Chapter::Prologue)
            orbitYaw += PROLOGUE_ORBIT_SPEED * deltaTime;
        else if(!playerMovedCamera)
        {
            float offset = std::remainder(orbitYaw, 360.0f);
            orbitYaw = offset * std::exp(-0.8f * deltaTime);
        }

        orbitYaw -= mouseDelta.x;
        orbitPitch = glm::clamp(orbitPitch - mouseDelta.y, 3.0f, 60.0f);

        glm::vec3 shipPosition = scene.getShipPosition();
        glm::vec3 target = glm::vec3(shipPosition.x, CHASE_TARGET_HEIGHT, shipPosition.z);

        float angle = glm::radians(scene.getShipHeadingDegrees() + orbitYaw);
        float pitch = glm::radians(orbitPitch);
        glm::vec3 behind = glm::vec3(std::sin(angle), 0.0f, std::cos(angle));
        glm::vec3 desired = target + behind * orbitDistance * std::cos(pitch) + glm::vec3(0.0f, orbitDistance * std::sin(pitch), 0.0f);

        if(!chaseStarted)
        {
            chasePosition = desired;
            chaseStarted = true;
        }
        chasePosition = glm::mix(chasePosition, desired, 1.0f - std::exp(-CHASE_SMOOTHING * deltaTime));
        chasePosition.y = glm::max(chasePosition.y, 2.5f);

        camera.lookAt(chasePosition, target);
    }

    void Game::updateFreeCamera(float deltaTime, gps::Input& input, gps::Camera& camera)
    {
        glm::vec2 mouseDelta = input.getMouseDelta() * MOUSE_SENSITIVITY;
        camera.rotate(mouseDelta.y, mouseDelta.x);

        float velocity = FREE_CAMERA_SPEED * deltaTime;
        if(input.isKeyDown(GLFW_KEY_LEFT_SHIFT))
            velocity *= SPRINT_MULTIPLIER;

        if(input.isKeyDown(GLFW_KEY_W))
            camera.move(gps::MOVE_FORWARD, velocity);
        if(input.isKeyDown(GLFW_KEY_S))
            camera.move(gps::MOVE_BACKWARD, velocity);
        if(input.isKeyDown(GLFW_KEY_A))
            camera.move(gps::MOVE_LEFT, velocity);
        if(input.isKeyDown(GLFW_KEY_D))
            camera.move(gps::MOVE_RIGHT, velocity);
        if(input.isKeyDown(GLFW_KEY_SPACE))
            camera.move(gps::MOVE_UP, velocity);
        if(input.isKeyDown(GLFW_KEY_LEFT_CONTROL))
            camera.move(gps::MOVE_DOWN, velocity);
    }

    void Game::drawOverlay(gps::TextRenderer& text, gps::Scene& scene, int width, int height)
    {
        text.begin(width, height);
        story.draw(text, width, height);

        if(mode == ControlMode::Ship && story.getChapter() != Chapter::Prologue)
            drawShipStatus(text, scene, width, height);

        float scale = height / 600.0f;
        std::string hint;
        if(mode == ControlMode::Ship)
            hint = "W/S  sails     A/D  helm     mouse  look around     C  free camera";
        else
            hint = "free camera     W/A/S/D  move     Space/Ctrl  up/down     C  back to the ship";
        text.drawTextCentered(hint, width * 0.5f, height - text.lineHeight(scale) * 1.8f, scale, HINT_COLOR);

        text.end();
    }

    void Game::drawShipStatus(gps::TextRenderer& text, gps::Scene& scene, int width, int height)
    {
        float scale = height / 500.0f;
        char status[96];
        std::snprintf(status, sizeof(status), "sails %3d%%     speed %4.1f knots", (int)(scene.getShipSail() * 100.0f + 0.5f), scene.getShipSpeed() * 1.944f);
        text.drawText(status, text.lineHeight(scale), height - text.lineHeight(scale) * 3.2f, scale, HINT_COLOR);
    }

    std::string Game::describeMode()
    {
        if(mode == ControlMode::Ship)
            return "ship";
        return "free camera";
    }
}
