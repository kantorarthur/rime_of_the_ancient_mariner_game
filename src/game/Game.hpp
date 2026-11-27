#ifndef Game_hpp
#define Game_hpp

#include "core/Input.hpp"
#include "game/Cinematic.hpp"
#include "game/Story.hpp"
#include "graphics/TextRenderer.hpp"
#include "scene/Camera.hpp"
#include "scene/Scene.hpp"

#include <glm/glm.hpp>

#include <string>

namespace gps {

    enum class ControlMode {Ship, FreeCamera};

    class Game
    {
    public:
        void start(gps::Scene& scene);
        void update(float deltaTime, gps::Input& input, gps::Camera& camera, gps::Scene& scene);
        void drawOverlay(gps::TextRenderer& text, gps::Scene& scene, int width, int height);
        std::string describeMode();

    private:
        ControlMode mode = ControlMode::Ship;
        Story story;
        Cinematic cinematic;
        float fade = 0.0f;
        float fadeTarget = 0.0f;
        bool epilogueStarted = false;
        bool endReleased = false;
        bool albatrossFilmed = false;
        Chapter lastChapter = Chapter::Prologue;

        float orbitYaw = 150.0f;
        float orbitPitch = 10.0f;
        float orbitDistance = 60.0f;
        glm::vec3 chasePosition = glm::vec3(0.0f);
        bool chaseStarted = false;
        bool playerMovedCamera = false;

        void updateShip(gps::Input& input, gps::Scene& scene);
        void updateChaseCamera(float deltaTime, gps::Input& input, gps::Camera& camera, gps::Scene& scene);
        void updateFreeCamera(float deltaTime, gps::Input& input, gps::Camera& camera);
        void reactToStory(gps::Scene& scene);
        void filmShooting(gps::Scene& scene);
        void handleEvent(std::string event, gps::Scene& scene);
        void drawShipStatus(gps::TextRenderer& text, gps::Scene& scene, int width, int height);
    };
}

#endif
