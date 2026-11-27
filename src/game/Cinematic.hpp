#ifndef Cinematic_hpp
#define Cinematic_hpp

#include "graphics/TextRenderer.hpp"
#include "scene/Camera.hpp"
#include "scene/Scene.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace gps {

    enum class ShotFocus {Mariner, Albatross, AlbatrossFall, Crew, CrewFall, Person, ShipBow, ShipWide, GhostShip, GhostDeck, Dice};

    struct Shot
    {
        ShotFocus focus;
        int index;
        float duration;
    };

    class Cinematic
    {
    public:
        void play(std::vector<Shot> newShots);
        bool isPlaying();
        void update(float deltaTime, gps::Camera& camera, gps::Scene& scene);
        void drawBars(gps::TextRenderer& text, int width, int height);

    private:
        std::vector<Shot> shots;
        size_t current = 0;
        float shotTime = 0.0f;
        float barAmount = 0.0f;
        glm::vec3 anchor = glm::vec3(0.0f);

        void frame(Shot& shot, gps::Camera& camera, gps::Scene& scene);
        void framePerson(int character, float progress, glm::vec3& eye, glm::vec3& target, gps::Scene& scene);
        void startShot(gps::Scene& scene);
    };
}

#endif
