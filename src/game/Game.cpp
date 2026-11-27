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
    const glm::vec4 PROMPT_COLOR = glm::vec4(0.95f, 0.55f, 0.45f, 0.95f);

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

        if(input.wasKeyPressed(GLFW_KEY_ENTER))
            story.skipCaption();

        story.update(deltaTime, glm::length(glm::vec2(scene.getShipPosition().x, scene.getShipPosition().z)));
        reactToStory(scene);
        for(std::string& event : story.takeEvents())
            handleEvent(event, scene);

        if(story.getChapter() == Chapter::GhostApproach && scene.isGhostShipAlongside())
            story.ghostArrived();
        if(story.getChapter() == Chapter::Blessing && input.wasKeyPressed(GLFW_KEY_B))
        {
            scene.marinerBlesses();
            story.blessed();
        }
        if(story.getChapter() == Chapter::Rescue && scene.isBoatAtMariner())
            story.boatArrived();
        if(story.getChapter() == Chapter::Returning && scene.isBoatHome())
            story.landed();

        if(story.getChapter() == Chapter::Crossbow && input.wasKeyPressed(GLFW_KEY_X) && scene.raiseCrossbow())
            filmShooting(scene);
        if(story.getChapter() == Chapter::Crossbow && scene.isAlbatrossShot())
            story.albatrossShot();

        if(story.getChapter() == Chapter::Albatross && !albatrossFilmed && scene.getAlbatrossArrival() > 0.8f)
        {
            albatrossFilmed = true;
            cinematic.play({{ShotFocus::Albatross, 0, 6.0f}, {ShotFocus::Crew, 0, 2.5f}, {ShotFocus::Crew, 2, 2.5f}});
        }

        if(cinematic.isPlaying())
        {
            scene.setShipControls(0.0f, 0.0f);
            cinematic.update(deltaTime, camera, scene);
            return;
        }
        cinematic.update(deltaTime, camera, scene);

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
        {
            scene.setWeather(Weather::Storm);
            cinematic.play({{ShotFocus::ShipBow, 0, 7.0f}});
        }

        if(chapter == Chapter::Albatross)
        {
            scene.callAlbatross();
            scene.crewHailAlbatross();
            scene.setWeather(Weather::Calm);
        }

        if(chapter == Chapter::Shot)
            scene.crewMourn();

        if(chapter == Chapter::Becalmed)
        {
            scene.setSailingWind(0.0f);
            cinematic.play({{ShotFocus::ShipWide, 0, 10.0f}});
        }

        lastChapter = chapter;
    }

    void Game::handleEvent(std::string event, gps::Scene& scene)
    {
        int death = scene.getDeathCharacter();
        int life = scene.getLifeInDeathCharacter();

        if(event == "deathfires")
        {
            scene.lightDeathFires();
            cinematic.play({{ShotFocus::ShipWide, 0, 9.0f}});
        }
        else if(event == "neck")
        {
            scene.hangAlbatross();
            cinematic.play({{ShotFocus::Mariner, 0, 6.0f}});
        }
        else if(event == "sail")
        {
            scene.marinerBitesArm();
            scene.summonGhostShip();
            cinematic.play({{ShotFocus::Mariner, 0, 4.0f}, {ShotFocus::GhostShip, 0, 7.0f}});
        }
        else if(event == "twain")
            cinematic.play({{ShotFocus::GhostShip, 0, 4.0f}, {ShotFocus::GhostDeck, 0, 4.0f}, {ShotFocus::Person, death, 3.0f}});
        else if(event == "lifeface")
            cinematic.play({{ShotFocus::Person, life, 11.0f}});
        else if(event == "dice")
        {
            scene.throwDice();
            cinematic.play({{ShotFocus::Dice, 0, 7.0f}});
        }
        else if(event == "won")
        {
            scene.lifeInDeathWins();
            cinematic.play({{ShotFocus::Person, life, 4.0f}, {ShotFocus::GhostDeck, 0, 4.0f}});
        }
        else if(event == "ghostleaves")
        {
            scene.dismissGhostShip();
            cinematic.play({{ShotFocus::GhostShip, 0, 8.0f}});
        }
        else if(event == "cursed")
        {
            scene.crewCurse();
            std::vector<Shot> shots;
            for(int i = 0; i < (int)scene.getCrewCharacters().size(); i++)
                shots.push_back({ShotFocus::Crew, i, 2.5f});
            cinematic.play(shots);
        }
        else if(event == "deaths")
        {
            scene.killCrew(2.6f);
            std::vector<Shot> shots;
            for(int i = 0; i < (int)scene.getCrewCharacters().size(); i++)
                shots.push_back({ShotFocus::CrewFall, i, 2.6f});
            shots.push_back({ShotFocus::Mariner, 0, 4.0f});
            cinematic.play(shots);
        }
        else if(event == "snakes")
        {
            scene.showWaterSnakes(true);
            cinematic.play({{ShotFocus::Snakes, 0, 5.5f}, {ShotFocus::Snakes, 5, 5.5f}});
        }
        else if(event == "blessed")
        {
            scene.liftCurse();
            cinematic.play({{ShotFocus::Mariner, 0, 5.0f}, {ShotFocus::Snakes, 2, 3.0f}});
        }
        else if(event == "albatrossfalls")
        {
            scene.dropHungAlbatross();
            cinematic.play({{ShotFocus::Overboard, 0, 6.0f}});
        }
        else if(event == "rain")
        {
            scene.showWaterSnakes(false);
            scene.setWeather(Weather::Storm);
            cinematic.play({{ShotFocus::ShipWide, 0, 8.0f}});
        }
        else if(event == "lightning")
            scene.triggerLightning(scene.shipPoint(glm::vec3(0.0f, 10.0f, 0.0f)), scene.shipPoint(glm::vec3(0.0f, 0.0f, 1.0f)) - scene.shipPoint(glm::vec3(0.0f)));
        else if(event == "rise")
        {
            scene.crewRise();
            std::vector<Shot> shots;
            for(int i = 0; i < (int)scene.getCrewCharacters().size(); i++)
                shots.push_back({ShotFocus::CrewFall, i, 2.4f});
            cinematic.play(shots);
        }
        else if(event == "spiritwind")
        {
            scene.setWeather(Weather::Calm);
            scene.setSailingWind(1.0f);
        }
        else if(event == "home")
        {
            scene.launchBoat();
            cinematic.play({{ShotFocus::Lighthouse, 0, 5.0f}, {ShotFocus::Boat, 0, 4.0f}});
        }
        else if(event == "seraphs")
        {
            scene.setSailingWind(0.0f);
            scene.crewFallAgain();
            std::vector<Shot> shots;
            for(int i = 0; i < 2; i++)
                shots.push_back({ShotFocus::CrewFall, i, 3.5f});
            cinematic.play(shots);
        }
        else if(event == "sink")
        {
            scene.sinkShip();
            cinematic.play({{ShotFocus::Sinking, 0, 10.0f}});
        }
        else if(event == "afloat")
        {
            scene.marinerAfloat();
            cinematic.play({{ShotFocus::Swimmer, scene.getMarinerCharacter(), 6.0f}, {ShotFocus::Boat, 0, 120.0f}});
        }
        else if(event == "aboard")
        {
            scene.marinerAboard();
            cinematic.play({{ShotFocus::Boat, 0, 120.0f}});
        }
        else if(event == "pilotfit")
        {
            scene.pilotFit();
            cinematic.play({{ShotFocus::BoatPerson, scene.getPilotCharacter(), 5.0f}, {ShotFocus::BoatPerson, scene.getHermitCharacter(), 5.0f}, {ShotFocus::Boat, 0, 120.0f}});
        }
        else if(event == "boylaughs")
        {
            scene.boyLaughs();
            cinematic.play({{ShotFocus::BoatPerson, scene.getPilotBoyCharacter(), 5.0f}, {ShotFocus::Boat, 0, 300.0f}});
        }
        else if(event == "ashore")
        {
            scene.marinerAshore();
            cinematic.play({{ShotFocus::Person, scene.getMarinerCharacter(), 8.0f}, {ShotFocus::Lighthouse, 0, 1000.0f}});
        }
        else if(event == "alone")
        {
            scene.marinerDespairs();
            cinematic.play({{ShotFocus::ShipWide, 0, 11.0f}});
        }
    }

    void Game::filmShooting(gps::Scene& scene)
    {
        std::vector<Shot> shots = {{ShotFocus::Mariner, 0, 1.9f}, {ShotFocus::AlbatrossFall, 0, 3.6f}};
        for(int i = 0; i < (int)scene.getCrewCharacters().size(); i++)
            shots.push_back({ShotFocus::Crew, i, 2.4f});
        cinematic.play(shots);
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
        cinematic.drawBars(text, width, height);
        story.draw(text, width, height);

        if(cinematic.isPlaying())
        {
            text.end();
            return;
        }

        if(mode == ControlMode::Ship && story.getChapter() != Chapter::Prologue && !scene.isShipSinking())
            drawShipStatus(text, scene, width, height);

        float promptScale = height / 300.0f;
        if(story.getChapter() == Chapter::Crossbow)
            text.drawTextCentered("X   take up the cross-bow", width * 0.5f, height * 0.82f, promptScale, PROMPT_COLOR);
        if(story.getChapter() == Chapter::Blessing)
            text.drawTextCentered("B   bless them unaware", width * 0.5f, height * 0.82f, promptScale, PROMPT_COLOR);
        if(story.getChapter() == Chapter::Homeward)
            text.drawTextCentered("sail home to the lighthouse", width * 0.5f, height * 0.82f, promptScale, PROMPT_COLOR);

        float scale = height / 600.0f;
        std::string hint;
        if(mode == ControlMode::Ship)
            hint = "W/S  sails     A/D  helm     mouse  look around     C  free camera     Enter  next verse";
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
        std::string line = status;
        if(scene.getSailingWind() <= 0.0f)
            line += "     no wind - the ship is becalmed";
        if(story.getChapter() == Chapter::Homeward)
        {
            glm::vec3 ship = scene.getShipPosition();
            char home[48];
            std::snprintf(home, sizeof(home), "     lighthouse %d m", (int)glm::length(glm::vec2(ship.x, ship.z)));
            line += home;
        }
        text.drawText(line, text.lineHeight(scale), height - text.lineHeight(scale) * 3.2f, scale, HINT_COLOR);
    }

    std::string Game::describeMode()
    {
        if(mode == ControlMode::Ship)
            return "ship";
        return "free camera";
    }
}
