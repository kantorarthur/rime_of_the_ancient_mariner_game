#include "game/Cinematic.hpp"

namespace gps {

    const float BAR_SPEED = 2.5f;
    const float BAR_HEIGHT = 0.11f;

    void Cinematic::play(std::vector<Shot> newShots)
    {
        shots = newShots;
        current = 0;
        shotTime = 0.0f;
    }

    bool Cinematic::isPlaying()
    {
        return current < shots.size();
    }

    void Cinematic::startShot(gps::Scene& scene)
    {
        Shot& shot = shots[current];
        if(shot.focus == ShotFocus::Sinking)
        {
            glm::vec3 ship = scene.shipPoint(glm::vec3(0.0f));
            glm::vec3 side = scene.shipPoint(glm::vec3(1.0f, 0.0f, 0.0f)) - ship;
            anchor = glm::vec3(ship.x, 0.0f, ship.z) + glm::normalize(glm::vec3(side.x, 0.0f, side.z)) * 55.0f + glm::vec3(0.0f, 7.0f, 0.0f);
        }
        if(shot.focus == ShotFocus::Overboard)
            anchor = scene.shipPoint(glm::vec3(-9.0f, 3.5f, -10.0f));
        if(shot.focus == ShotFocus::AlbatrossFall)
        {
            glm::vec3 bird = scene.getAlbatrossPosition();
            glm::vec3 ship = scene.shipPoint(glm::vec3(0.0f));
            glm::vec3 towardShip = glm::normalize(glm::vec3(ship.x - bird.x, 0.0f, ship.z - bird.z));
            glm::vec3 side = glm::cross(towardShip, glm::vec3(0.0f, 1.0f, 0.0f));
            anchor = bird + towardShip * 9.0f + side * 5.0f;
        }
    }

    void Cinematic::update(float deltaTime, gps::Camera& camera, gps::Scene& scene)
    {
        float barTarget = isPlaying() ? 1.0f : 0.0f;
        float step = BAR_SPEED * deltaTime;
        barAmount += glm::clamp(barTarget - barAmount, -step, step);

        if(!isPlaying())
            return;

        if(shotTime == 0.0f)
            startShot(scene);

        if(shots[current].focus == ShotFocus::Boat)
        {
            float right = scene.distanceToPier(scene.getBoatPoint(glm::vec3(3.8f, 0.0f, 4.6f)));
            float left = scene.distanceToPier(scene.getBoatPoint(glm::vec3(-3.8f, 0.0f, 4.6f)));
            float sideTarget = right >= left ? 1.0f : -1.0f;
            if(shotTime == 0.0f)
                boatSide = sideTarget;
            float sideStep = 0.8f * deltaTime;
            boatSide += glm::clamp(sideTarget - boatSide, -sideStep, sideStep);
        }

        frame(shots[current], camera, scene);

        shotTime += deltaTime;
        if(shotTime >= shots[current].duration)
        {
            current++;
            shotTime = 0.0f;
        }
    }

    void Cinematic::frame(Shot& shot, gps::Camera& camera, gps::Scene& scene)
    {
        float progress = glm::clamp(shotTime / shot.duration, 0.0f, 1.0f);
        glm::vec3 eye;
        glm::vec3 target;

        if(shot.focus == ShotFocus::Mariner)
        {
            int mariner = scene.getMarinerCharacter();
            glm::vec3 head = scene.characterHead(mariner);
            float distance = glm::mix(2.6f, 1.9f, progress);
            eye = scene.characterPoint(mariner, glm::vec3(-0.9f, 0.0f, distance));
            eye.y = head.y + 0.1f;
            target = head + glm::vec3(0.0f, -0.35f, 0.0f);
        }
        else if(shot.focus == ShotFocus::Albatross)
        {
            glm::vec3 bird = scene.getAlbatrossPosition();
            glm::vec3 velocity = scene.getAlbatrossVelocity();
            glm::vec3 heading = glm::length(velocity) > 0.1f ? glm::normalize(velocity) : glm::vec3(0.0f, 0.0f, 1.0f);
            glm::vec3 side = glm::normalize(glm::cross(heading, glm::vec3(0.0f, 1.0f, 0.0f)));
            eye = bird - heading * glm::mix(9.0f, 6.0f, progress) + side * 3.5f + glm::vec3(0.0f, 1.8f, 0.0f);
            target = bird;
        }
        else if(shot.focus == ShotFocus::AlbatrossFall)
        {
            glm::vec3 bird = scene.getAlbatrossPosition();
            glm::vec3 closer = glm::mix(anchor, bird, 0.45f * progress);
            eye = glm::vec3(closer.x, glm::max(bird.y + 2.0f, 1.6f), closer.z);
            target = bird;
        }
        else if(shot.focus == ShotFocus::Crew)
            framePerson(scene.getCrewCharacters()[shot.index], progress, eye, target, scene);
        else if(shot.focus == ShotFocus::CrewFall)
        {
            int sailor = scene.getCrewCharacters()[shot.index];
            eye = scene.characterPoint(sailor, glm::vec3(1.2f, 2.4f, 2.8f));
            target = glm::mix(scene.characterHead(sailor), scene.characterPoint(sailor, glm::vec3(0.0f, 0.6f, -0.4f)), 0.5f);
        }
        else if(shot.focus == ShotFocus::Person)
            framePerson(shot.index, progress, eye, target, scene);
        else if(shot.focus == ShotFocus::GhostShip)
        {
            eye = scene.shipPoint(glm::vec3(-3.5f, 9.5f, -9.0f));
            target = scene.getGhostShipPoint(glm::vec3(0.0f, 9.0f, 0.0f));
        }
        else if(shot.focus == ShotFocus::GhostDeck)
        {
            eye = scene.getGhostShipPoint(glm::vec3(glm::mix(-7.0f, -5.5f, progress), 8.0f, 5.5f));
            target = scene.getGhostShipPoint(glm::vec3(0.0f, 5.3f, 0.0f));
        }
        else if(shot.focus == ShotFocus::Swimmer)
        {
            glm::vec3 head = scene.characterHead(shot.index);
            glm::vec3 front = scene.characterPoint(shot.index, glm::vec3(0.0f, 0.0f, 1.0f)) - scene.characterPoint(shot.index, glm::vec3(0.0f));
            front = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
            eye = head + front * glm::mix(5.0f, 3.2f, progress) + glm::vec3(0.0f, glm::mix(2.6f, 1.4f, progress), 0.0f);
            target = head - glm::vec3(0.0f, 0.4f, 0.0f);
        }
        else if(shot.focus == ShotFocus::BoatPerson)
        {
            glm::vec3 head = scene.characterHead(shot.index);
            glm::vec3 side = scene.getBoatPoint(glm::vec3(1.0f, 0.0f, 0.0f)) - scene.getBoatPoint(glm::vec3(0.0f));
            glm::vec3 forward = scene.getBoatPoint(glm::vec3(0.0f, 0.0f, 1.0f)) - scene.getBoatPoint(glm::vec3(0.0f));
            eye = head + side * glm::mix(3.4f, 2.6f, progress) + forward * 1.2f + glm::vec3(0.0f, 0.5f, 0.0f);
            target = head - glm::vec3(0.0f, 0.15f, 0.0f);
        }
        else if(shot.focus == ShotFocus::Overboard)
        {
            eye = anchor;
            target = glm::mix(scene.characterHead(scene.getMarinerCharacter()), scene.getAlbatrossPosition(), glm::smoothstep(0.0f, 0.4f, progress));
        }
        else if(shot.focus == ShotFocus::Snakes)
        {
            glm::vec3 snake = scene.getSnakeHead(shot.index);
            glm::vec3 ship = scene.shipPoint(glm::vec3(0.0f));
            glm::vec3 outward = glm::normalize(glm::vec3(snake.x - ship.x, 0.0f, snake.z - ship.z));
            eye = snake + outward * glm::mix(8.0f, 5.5f, progress) + glm::vec3(0.0f, 3.0f, 0.0f);
            target = snake + glm::vec3(0.0f, 0.5f, 0.0f);
        }
        else if(shot.focus == ShotFocus::Lighthouse)
        {
            if(scene.isShipSinking())
                eye = scene.getBoatPoint(glm::vec3(2.5f, 3.0f, -7.0f));
            else
                eye = scene.shipPoint(glm::vec3(2.5f, 11.0f, -13.0f));
            target = scene.getLighthouseTop();
        }
        else if(shot.focus == ShotFocus::Sinking)
        {
            glm::vec3 ship = scene.getShipPosition();
            eye = anchor;
            target = glm::vec3(ship.x, 2.0f, ship.z);
        }
        else if(shot.focus == ShotFocus::Boat)
        {
            eye = scene.getBoatPoint(glm::vec3(boatSide * 3.8f, 3.4f, glm::mix(5.0f, 4.2f, progress)));
            target = scene.getBoatPoint(glm::vec3(0.0f, 0.9f, 0.0f));
        }
        else if(shot.focus == ShotFocus::Dice)
        {
            eye = scene.getGhostShipPoint(glm::vec3(-0.5f, 6.3f, glm::mix(1.6f, 1.2f, progress)));
            target = scene.getGhostShipPoint(glm::vec3(0.1f, 5.1f, 0.0f));
        }
        else if(shot.focus == ShotFocus::ShipBow)
        {
            eye = scene.shipPoint(glm::vec3(10.0f, 7.0f, glm::mix(32.0f, 26.0f, progress)));
            target = scene.shipPoint(glm::vec3(0.0f, 9.0f, 0.0f));
        }
        else
        {
            eye = scene.shipPoint(glm::vec3(glm::mix(-55.0f, -45.0f, progress), 10.0f, 18.0f));
            target = scene.shipPoint(glm::vec3(0.0f, 8.0f, 0.0f));
        }

        camera.lookAt(eye, target);
    }

    void Cinematic::framePerson(int character, float progress, glm::vec3& eye, glm::vec3& target, gps::Scene& scene)
    {
        glm::vec3 head = scene.characterHead(character);
        glm::vec3 front = scene.characterPoint(character, glm::vec3(0.0f, 0.0f, 1.0f)) - scene.characterPoint(character, glm::vec3(0.0f));
        glm::vec3 side = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
        eye = head + front * glm::mix(2.1f, 1.6f, progress) + side * 0.6f + glm::vec3(0.0f, 0.15f, 0.0f);
        target = head + glm::vec3(0.0f, -0.25f, 0.0f);
    }

    void Cinematic::drawBars(gps::TextRenderer& text, int width, int height)
    {
        if(barAmount <= 0.0f)
            return;

        float bar = height * BAR_HEIGHT * glm::smoothstep(0.0f, 1.0f, barAmount);
        text.drawRectangle(0.0f, 0.0f, (float)width, bar, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        text.drawRectangle(0.0f, height - bar, (float)width, bar, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    }
}
