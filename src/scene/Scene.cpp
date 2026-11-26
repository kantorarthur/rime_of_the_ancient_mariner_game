#include "scene/Scene.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <string>

namespace gps {

    const float PIER_DECK_HEIGHT = 1.2f;
    const float PIER_MODEL_DECK = 3.6f;
    const float PIER_SECTION_LENGTH = 3.39f;
    const float PIER_ANGLE = 330.0f;
    const float CLIFF_RING_START = 31.0f;
    const float CLIFF_RING_END = 292.0f;

    const glm::vec3 LIGHTHOUSE_POSITION = glm::vec3(6.0f, 3.3f, -6.0f);
    const float LIGHTHOUSE_LAMP_HEIGHT = 16.3f;
    const float BEAM_SPEED_DEGREES = 30.0f;
    const float BEAM_TILT = -0.12f;
    const float BEAM_SHAFT_LENGTH = 150.0f;
    const float BEAM_SHAFT_RADIUS = 0.6f;
    const float BEAM_SHAFT_SPREAD = 4.5f;

    const int MOON_SHADOW_SIZE = 4096;
    const int BEAM_SHADOW_SIZE = 2048;
    const float MOON_SHADOW_RADIUS = 62.0f;
    const float MOON_SHADOW_DISTANCE = 150.0f;
    const GLuint MOON_SHADOW_UNIT = 5;
    const GLuint BEAM_SHADOW_UNIT = 6;

    const glm::vec2 SHIP_START_POSITION = glm::vec2(30.0f, 75.0f);
    const float SHIP_START_HEADING = 200.0f;
    const float SHIP_DRAFT = 3.2f;
    const float SHIP_MODEL_YAW = 180.0f;
    const float SHIP_LENGTH = 30.0f;
    const float SHIP_WIDTH = 7.5f;
    const float ISLAND_RADIUS = 23.0f;
    const glm::vec3 SHIP_LANTERN = glm::vec3(0.0f, 14.5f, -16.0f);

    const float ALBATROSS_RADIUS = 26.0f;
    const float ALBATROSS_HEIGHT = 25.0f;
    const float ALBATROSS_SPEED = 9.0f;
    const float ALBATROSS_BANK = 28.0f;
    const float ALBATROSS_FLAP_SPEED = 2.2f;
    const float ALBATROSS_SHIP_RADIUS = 24.0f;
    const float ALBATROSS_SHIP_HEIGHT = 24.0f;
    const float ALBATROSS_ARRIVAL_TIME = 15.0f;
    const float CHARACTER_TURN_SPEED = 150.0f;
    const float CROSSBOW_AIM_TIME = 1.0f;
    const float CROSSBOW_LOWER_TIME = 3.5f;
    const glm::mat4 CROSSBOW_GRIP = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.06f));

    const std::string CHARACTER_FOLDER = "assets/characters/";
    const char* CHARACTER_MESHES[] = {"mariner", "sailor_1", "sailor_2", "sailor_3", "sailor_4"};
    const char* ANIMATION_CLIPS[] = {"idle", "talk", "helm", "death", "sit", "sit_talk", "aim", "shoot", "bless", "bless_hold", "grieve", "kneel", "dance", "walk", "walk_formal", "swim", "hit", "interact", "haul", "rail", "rail_call", "shake_head", "fold_arms", "lantern", "rise", "zombie", "zombie_walk", "yes", "throw", "consume"};

    const int RAIN_DROP_COUNT = 12000;
    const glm::vec3 LIGHTNING_COLOR = glm::vec3(1.4f, 1.5f, 1.8f);
    const glm::vec3 LIGHTNING_AMBIENT = glm::vec3(0.10f, 0.11f, 0.14f);

    const glm::vec3 CALM_MOON_DIRECTION = glm::vec3(0.378f, 0.378f, -0.845f);
    const glm::vec3 STORM_MOON_DIRECTION = glm::vec3(0.399f, 0.227f, -0.888f);

    glm::mat4 SceneObject::getModelMatrix()
    {
        glm::mat4 matrix = glm::translate(glm::mat4(1.0f), position);
        matrix = glm::rotate(matrix, glm::radians(rotationDegrees.y), glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotationDegrees.x), glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotationDegrees.z), glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, glm::vec3(scale));
        return matrix;
    }

    void Scene::load()
    {
        loadModels();
        placeObjects();
        loadCharacters();
        placeCrew();
        placeObstacles();
        placeLights();
        applyWeather();

        water.init(220, 256, 1500.0f);
        placeFoam();
        if(weather == Weather::Calm)
            stormFactor = 0.0f;
        water.update(0.0f, stormFactor);
        updateFloating();
        fog.update(0.0f, stormFactor, 0.0f);
        rain.init(RAIN_DROP_COUNT);
        lightning.init();
        lightBeam.init(BEAM_SHAFT_LENGTH, BEAM_SHAFT_RADIUS, BEAM_SHAFT_SPREAD, 48);
        moonShadowMap.init(MOON_SHADOW_SIZE);
        beamShadowMap.init(BEAM_SHADOW_SIZE);
    }

    void Scene::loadModels()
    {
        island.LoadModel("assets/models/island/island.obj");
        cliffRing.LoadModel("assets/models/cliff/cliff_ring.obj");
        lighthouse.LoadModel("assets/models/lighthouse/lighthouse.obj");
        house.LoadModel("assets/models/house/house.obj");
        pier.LoadModel("assets/models/pier/pier_a.obj");
        lantern.LoadModel("assets/models/lantern/lantern.obj");
        boat.LoadModel("assets/models/boat/boat.obj");
        ship.LoadModel("assets/models/ship/ship.obj");
        crate.LoadModel("assets/models/crate/crate.obj");
        albatross.LoadModel("assets/models/albatross/albatross.obj");
        crossbow.LoadModel("assets/models/crossbow/crossbow.obj");

        for(int i = 0; i < 3; i++)
            rocks[i].LoadModel("assets/models/rocks/rock_" + std::to_string(i + 1) + ".obj");
    }

    void Scene::placeObjects()
    {
        add(island, glm::vec3(0.0f), 0.0f, 1.0f);

        add(cliffRing, glm::vec3(0.0f), 0.0f, 1.0f);
        add(rocks[0], onCircle(CLIFF_RING_START, 21.0f, -0.9f), 40.0f, 2.6f);
        add(rocks[2], onCircle(CLIFF_RING_END, 21.0f, -0.9f), 160.0f, 2.6f);

        add(lighthouse, LIGHTHOUSE_POSITION, 0.0f, 1.0f).emissiveStrength = 4.0f;
        add(house, glm::vec3(-5.0f, 3.45f, -1.0f), 320.0f, 1.0f);

        float deckY = PIER_DECK_HEIGHT - PIER_MODEL_DECK;
        for(int i = 0; i < 4; i++)
            add(pier, onCircle(PIER_ANGLE, 21.5f + i * PIER_SECTION_LENGTH, deckY), PIER_ANGLE, 1.0f);

        glm::vec3 pierEnd = onCircle(PIER_ANGLE, 21.5f + 3.0f * PIER_SECTION_LENGTH + 1.0f, PIER_DECK_HEIGHT);
        glm::vec3 pierSide = glm::vec3(std::cos(glm::radians(PIER_ANGLE)), 0.0f, -std::sin(glm::radians(PIER_ANGLE)));
        add(lantern, pierEnd + pierSide * 0.9f, 0.0f, 1.0f).emissiveStrength = 1.5f;

        glm::vec3 boatPosition = onCircle(PIER_ANGLE, 28.0f, -0.3f) - pierSide * 3.0f;
        setFloating(add(boat, boatPosition, PIER_ANGLE + 180.0f, 1.0f), 0.3f, 4.4f, 1.8f);

        SceneObject& wreck = add(ship, glm::vec3(-46.0f, -5.5f, 18.0f), 65.0f, 1.0f);
        wreck.rotationDegrees.x = -7.0f;
        wreck.rotationDegrees.z = 21.0f;
        wreck.visible = false;
        wreckIndex = (int)objects.size() - 1;

        shipController.reset(SHIP_START_POSITION, SHIP_START_HEADING);
        setFloating(add(ship, glm::vec3(SHIP_START_POSITION.x, 0.0f, SHIP_START_POSITION.y), SHIP_START_HEADING + SHIP_MODEL_YAW, 1.0f), SHIP_DRAFT, SHIP_LENGTH, SHIP_WIDTH);
        shipIndex = (int)objects.size() - 1;

        setFloating(add(crate, glm::vec3(-26.0f, 0.0f, 22.0f), 25.0f, 1.0f), 0.22f, 1.17f, 0.53f);
        setFloating(add(crate, glm::vec3(-16.0f, 0.0f, 40.0f), 70.0f, 1.0f), 0.22f, 1.17f, 0.53f);
        setFloating(add(crate, glm::vec3(8.0f, 0.0f, 36.0f), 140.0f, 1.0f), 0.22f, 1.17f, 0.53f);
        setFloating(add(crate, glm::vec3(-38.0f, 0.0f, -4.0f), 200.0f, 1.0f), 0.22f, 1.17f, 0.53f);

        albatrossPrevious = albatrossPathPoint(LIGHTHOUSE_POSITION, ALBATROSS_RADIUS, ALBATROSS_HEIGHT);
        add(albatross, albatrossPrevious, 0.0f, 1.0f);
        albatrossIndex = (int)objects.size() - 1;

        add(rocks[0], glm::vec3(-34.0f, -0.8f, 8.0f), 0.0f, 1.6f);
        add(rocks[1], glm::vec3(-42.0f, -1.0f, -10.0f), 50.0f, 2.2f);
        add(rocks[2], glm::vec3(-27.0f, -0.9f, 32.0f), 120.0f, 1.3f);
        add(rocks[0], glm::vec3(31.0f, -0.8f, 27.0f), 200.0f, 1.8f);
        add(rocks[1], glm::vec3(39.0f, -1.0f, -6.0f), 300.0f, 2.0f);
        add(rocks[2], glm::vec3(-10.0f, -0.8f, 44.0f), 80.0f, 1.4f);
    }

    void Scene::loadCharacters()
    {
        for(const char* name : CHARACTER_MESHES)
        {
            skinnedMeshes[name] = std::make_unique<gps::SkinnedMesh>();
            skinnedMeshes[name]->load(CHARACTER_FOLDER + name + ".skin", CHARACTER_FOLDER + "textures");
        }
        for(const char* name : ANIMATION_CLIPS)
        {
            clips[name] = std::make_unique<gps::AnimationClip>();
            clips[name]->load(CHARACTER_FOLDER + "animations/" + name + ".anim");
        }
    }

    int Scene::addCharacter(std::string meshName, glm::vec3 localPosition, float localYaw, int parentObject)
    {
        Character character;
        character.mesh = skinnedMeshes[meshName].get();
        character.animator.attach(character.mesh);
        character.localPosition = localPosition;
        character.localYaw = localYaw;
        character.targetYaw = localYaw;
        character.parentObject = parentObject;
        characters.push_back(character);
        return (int)characters.size() - 1;
    }

    void Scene::playAnimation(int character, std::string clipName, bool loop, float blendTime)
    {
        characters[character].animator.play(clips[clipName].get(), loop, blendTime);
    }

    void Scene::placeCrew()
    {
        marinerCharacter = addCharacter("mariner", glm::vec3(0.0f, 6.5f, -7.6f), 0.0f, shipIndex);
        playAnimation(marinerCharacter, "helm", true, 0.0f);
        Character& mariner = characters[marinerCharacter];
        mariner.prop = &crossbow;
        mariner.propBone = mariner.mesh->findBone("hand_r");
        mariner.propOffset = CROSSBOW_GRIP;

        crewCharacters.push_back(addCharacter("sailor_1", glm::vec3(2.0f, 4.65f, 1.0f), 90.0f, shipIndex));
        crewCharacters.push_back(addCharacter("sailor_2", glm::vec3(-2.0f, 4.65f, -1.0f), -90.0f, shipIndex));
        crewCharacters.push_back(addCharacter("sailor_3", glm::vec3(0.8f, 6.3f, 6.0f), 0.0f, shipIndex));
        crewCharacters.push_back(addCharacter("sailor_4", glm::vec3(-1.0f, 4.65f, 2.5f), 90.0f, shipIndex));

        const char* work[] = {"rail", "rail_call", "fold_arms", "haul"};
        for(size_t i = 0; i < crewCharacters.size(); i++)
            playAnimation(crewCharacters[i], work[i], true, 0.0f);
    }

    glm::mat4 Scene::characterMatrix(Character& character)
    {
        glm::mat4 parent = glm::mat4(1.0f);
        if(character.parentObject >= 0)
            parent = objects[character.parentObject].getModelMatrix();
        glm::mat4 local = glm::translate(glm::mat4(1.0f), character.localPosition);
        local = glm::rotate(local, glm::radians(character.localYaw), glm::vec3(0.0f, 1.0f, 0.0f));
        local = glm::scale(local, glm::vec3(character.scale));
        return parent * local;
    }

    void Scene::drawCharacters(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition)
    {
        uploadEnvironment(shader, cameraPosition);
        GLuint program = shader.shaderProgram;
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1f(glGetUniformLocation(program, "emissiveStrength"), 0.0f);
        glUniform1f(glGetUniformLocation(program, "shininess"), 16.0f);

        for(Character& character : characters)
        {
            if(!character.visible)
                continue;
            glm::mat4 model = characterMatrix(character);
            glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(model));
            glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix3fv(glGetUniformLocation(program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
            character.mesh->draw(shader, character.animator.getPalette());
        }
    }

    void Scene::drawCharacterDepth(gps::Shader shader, glm::mat4 lightSpace)
    {
        shader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "lightSpace"), 1, GL_FALSE, glm::value_ptr(lightSpace));
        for(Character& character : characters)
        {
            if(!character.visible)
                continue;
            glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(characterMatrix(character)));
            character.mesh->draw(shader, character.animator.getPalette());
        }
    }

    void Scene::placeObstacles()
    {
        obstacles.push_back({glm::vec2(0.0f), ISLAND_RADIUS});

        for(int i = 0; i < 4; i++)
        {
            glm::vec3 pierPoint = onCircle(PIER_ANGLE, 23.0f + i * PIER_SECTION_LENGTH, 0.0f);
            obstacles.push_back({glm::vec2(pierPoint.x, pierPoint.z), 2.0f});
        }

        for(SceneObject& object : objects)
        {
            bool isRock = object.model == &rocks[0] || object.model == &rocks[1] || object.model == &rocks[2];
            bool isBoat = object.model == &boat;
            if(isRock)
                obstacles.push_back({glm::vec2(object.position.x, object.position.z), 1.2f * object.scale});
            if(isBoat)
                obstacles.push_back({glm::vec2(object.position.x, object.position.z), 2.4f});
        }
    }

    void Scene::placeLights()
    {
        glm::vec3 lamp = LIGHTHOUSE_POSITION + glm::vec3(0.0f, LIGHTHOUSE_LAMP_HEIGHT, 0.0f);
        pointLights.push_back({lamp, glm::vec3(1.0f, 0.78f, 0.45f) * 3.0f, 1.0f, 0.35f, 0.12f});

        SceneObject* houseObject = nullptr;
        SceneObject* lanternObject = nullptr;
        for(SceneObject& object : objects)
        {
            if(object.model == &house)
                houseObject = &object;
            if(object.model == &lantern)
                lanternObject = &object;
        }

        glm::vec3 windowColor = glm::vec3(1.0f, 0.58f, 0.24f) * 1.6f;
        pointLights.push_back({toWorld(*houseObject, glm::vec3(-2.2f, 1.7f, 3.6f)), windowColor, 1.0f, 0.45f, 0.25f});
        pointLights.push_back({toWorld(*houseObject, glm::vec3(2.2f, 1.7f, 3.6f)), windowColor, 1.0f, 0.45f, 0.25f});

        glm::vec3 lanternLight = lanternObject->position + glm::vec3(0.0f, 0.35f, 0.0f);
        pointLights.push_back({lanternLight, glm::vec3(1.0f, 0.66f, 0.3f) * 2.2f, 1.0f, 0.3f, 0.18f});

        pointLights.push_back({toWorld(objects[shipIndex], SHIP_LANTERN), glm::vec3(1.0f, 0.68f, 0.35f) * 3.0f, 1.0f, 0.12f, 0.03f});
        shipLanternLight = (int)pointLights.size() - 1;

        beam.position = lamp;
        beam.color = glm::vec3(1.0f, 0.92f, 0.72f) * 3.5f;
        beam.cutOffDegrees = 5.0f;
        beam.outerCutOffDegrees = 9.0f;
        beam.constant = 1.0f;
        beam.linear = 0.0f;
        beam.quadratic = 0.0004f;
    }

    void Scene::applyWeather()
    {
        if(weather == Weather::Calm)
        {
            moonBase.direction = CALM_MOON_DIRECTION;
            moonBase.color = glm::vec3(0.30f, 0.36f, 0.50f);
            moonBase.ambient = glm::vec3(0.035f, 0.045f, 0.065f);
        }
        else
        {
            moonBase.direction = STORM_MOON_DIRECTION;
            moonBase.color = glm::vec3(0.10f, 0.12f, 0.16f);
            moonBase.ambient = glm::vec3(0.018f, 0.022f, 0.03f);
        }
        moon = moonBase;
    }

    void Scene::toggleWeather()
    {
        if(weather == Weather::Calm)
            weather = Weather::Storm;
        else
            weather = Weather::Calm;
        applyWeather();
    }

    void Scene::setWeather(Weather newWeather)
    {
        if(weather != newWeather)
            toggleWeather();
    }

    Weather Scene::getWeather()
    {
        return weather;
    }

    void Scene::applyLightning()
    {
        float flash = lightning.getFlash();
        float weight = glm::smoothstep(0.0f, 0.15f, flash);

        moon = moonBase;
        moon.direction = glm::normalize(glm::mix(glm::normalize(moonBase.direction), lightning.getDirection(), weight));
        moon.color += LIGHTNING_COLOR * flash;
        moon.ambient += LIGHTNING_AMBIENT * flash;
    }

    void Scene::triggerLightning(glm::vec3 cameraPosition, glm::vec3 viewDirection)
    {
        if(stormFactor > 0.5f)
            lightning.strike(cameraPosition, viewDirection);
    }

    void Scene::toggleFog()
    {
        fog.toggle();
    }

    bool Scene::isFogEnabled()
    {
        return fog.isEnabled();
    }

    void Scene::placeFoam()
    {
        water.addFoamCircle(glm::vec2(0.0f), 21.6f, 1.8f);
        if(objects[wreckIndex].visible)
            water.addFoamCircle(glm::vec2(-46.0f, 18.0f), 7.0f, 3.0f);

        for(SceneObject& object : objects)
        {
            bool isRock = object.model == &rocks[0] || object.model == &rocks[1] || object.model == &rocks[2];
            if(isRock && glm::length(glm::vec2(object.position.x, object.position.z)) > 25.0f)
                water.addFoamCircle(glm::vec2(object.position.x, object.position.z), 1.25f * object.scale, 0.9f * object.scale);
        }
    }

    void Scene::setFloating(SceneObject& object, float draft, float length, float width)
    {
        object.floatDraft = draft;
        object.floatLength = length;
        object.floatWidth = width;
    }

    void Scene::updateFloating()
    {
        for(SceneObject& object : objects)
        {
            if(object.floatLength <= 0.0f)
                continue;

            float yaw = glm::radians(object.rotationDegrees.y);
            glm::vec3 forward = glm::vec3(-std::sin(yaw), 0.0f, -std::cos(yaw)) * (object.floatLength * 0.5f);
            glm::vec3 right = glm::vec3(std::cos(yaw), 0.0f, -std::sin(yaw)) * (object.floatWidth * 0.5f);
            glm::vec3 center = object.position;

            float bow = water.heightAt(center.x + forward.x, center.z + forward.z);
            float stern = water.heightAt(center.x - forward.x, center.z - forward.z);
            float starboard = water.heightAt(center.x + right.x, center.z + right.z);
            float port = water.heightAt(center.x - right.x, center.z - right.z);

            object.position.y = (bow + stern + starboard + port) * 0.25f - object.floatDraft;
            object.rotationDegrees.x = glm::degrees(std::atan2(bow - stern, object.floatLength));
            object.rotationDegrees.z = glm::degrees(std::atan2(starboard - port, object.floatWidth));
        }
    }

    glm::vec3 Scene::albatrossPathPoint(glm::vec3 center, float radius, float height)
    {
        float wander = radius + 5.0f * std::sin(albatrossTime * 0.23f);
        float altitude = height + 3.5f * std::sin(albatrossTime * 0.41f) + 1.2f * stormFactor * std::sin(albatrossTime * 2.7f);
        return glm::vec3(center.x + std::cos(albatrossAngle) * wander, altitude, center.z + std::sin(albatrossAngle) * wander);
    }

    void Scene::flyAlbatross(float deltaTime)
    {
        SceneObject& bird = objects[albatrossIndex];

        glm::vec3 position = albatrossPathPoint(LIGHTHOUSE_POSITION, ALBATROSS_RADIUS, ALBATROSS_HEIGHT);
        if(albatrossState == AlbatrossState::FollowingShip)
        {
            albatrossFollow = glm::min(1.0f, albatrossFollow + deltaTime / ALBATROSS_ARRIVAL_TIME);
            glm::vec3 shipCircle = albatrossPathPoint(getShipPosition(), ALBATROSS_SHIP_RADIUS, ALBATROSS_SHIP_HEIGHT);
            position = glm::mix(position, shipCircle, glm::smoothstep(0.0f, 1.0f, albatrossFollow));
        }

        albatrossVelocity = (position - albatrossPrevious) / deltaTime;
        albatrossPrevious = position;

        float horizontalSpeed = glm::length(glm::vec2(albatrossVelocity.x, albatrossVelocity.z));
        float wobble = 10.0f * stormFactor * std::sin(albatrossTime * 3.1f);

        bird.position = position;
        bird.rotationDegrees.y = glm::degrees(std::atan2(-albatrossVelocity.x, -albatrossVelocity.z));
        bird.rotationDegrees.x = glm::degrees(std::atan2(albatrossVelocity.y, horizontalSpeed));
        bird.rotationDegrees.z = wobble - ALBATROSS_BANK;

        float cycle = glm::mix(6.0f, 3.5f, stormFactor);
        float phase = std::fmod(albatrossTime, cycle);
        float envelope = glm::smoothstep(0.0f, 0.25f, phase) * (1.0f - glm::smoothstep(1.2f, 1.5f, phase));
        float stroke = std::sin(2.0f * glm::pi<float>() * ALBATROSS_FLAP_SPEED * albatrossTime);
        bird.wingFlap = glm::radians(5.0f + envelope * 32.0f * stroke);
    }

    void Scene::dropAlbatross(float deltaTime)
    {
        SceneObject& bird = objects[albatrossIndex];
        float surface = water.heightAt(bird.position.x, bird.position.z) + 0.12f;

        if(albatrossState == AlbatrossState::Falling)
        {
            albatrossVelocity.y -= 9.81f * deltaTime;
            bird.position += albatrossVelocity * deltaTime;
            bird.rotationDegrees.z += 260.0f * deltaTime;
            bird.rotationDegrees.x -= 90.0f * deltaTime;
            bird.wingFlap = glm::radians(-25.0f);
            if(bird.position.y <= surface)
                albatrossState = AlbatrossState::Floating;
            return;
        }

        bird.position.y = surface;
        bird.position += glm::vec3(wind.getVelocity().x, 0.0f, wind.getVelocity().z) * 0.03f * deltaTime;
        bird.rotationDegrees.x = 0.0f;
        bird.rotationDegrees.z = 0.0f;
        bird.wingFlap = glm::radians(-4.0f);
    }

    void Scene::updateAlbatross(float deltaTime)
    {
        if(deltaTime <= 0.0f)
            return;

        albatrossTime += deltaTime;
        if(albatrossState == AlbatrossState::Falling || albatrossState == AlbatrossState::Floating)
        {
            dropAlbatross(deltaTime);
            return;
        }

        float radius = albatrossState == AlbatrossState::FollowingShip ? ALBATROSS_SHIP_RADIUS : ALBATROSS_RADIUS;
        albatrossAngle += ALBATROSS_SPEED / radius * deltaTime;
        flyAlbatross(deltaTime);
    }

    void Scene::callAlbatross()
    {
        if(albatrossState == AlbatrossState::CirclingLighthouse)
        {
            albatrossState = AlbatrossState::FollowingShip;
            albatrossFollow = 0.0f;
        }
    }

    bool Scene::isAlbatrossShot()
    {
        return albatrossState == AlbatrossState::Falling || albatrossState == AlbatrossState::Floating;
    }

    glm::vec3 Scene::toShipLocal(glm::vec3 worldPoint)
    {
        return glm::vec3(glm::inverse(objects[shipIndex].getModelMatrix()) * glm::vec4(worldPoint, 1.0f));
    }

    float Scene::yawTowards(Character& character, glm::vec3 localTarget)
    {
        glm::vec3 offset = localTarget - character.localPosition;
        return glm::degrees(std::atan2(offset.x, offset.z));
    }

    void Scene::crewHailAlbatross()
    {
        const char* greetings[] = {"rail_call", "rail_call", "yes", "yes"};
        for(size_t i = 0; i < crewCharacters.size(); i++)
            playAnimation(crewCharacters[i], greetings[i % 4], true, 0.5f);
    }

    void Scene::crewMourn()
    {
        const char* reactions[] = {"shake_head", "grieve", "shake_head", "grieve"};
        Character& mariner = characters[marinerCharacter];
        for(size_t i = 0; i < crewCharacters.size(); i++)
        {
            Character& sailor = characters[crewCharacters[i]];
            sailor.targetYaw = yawTowards(sailor, mariner.localPosition);
            playAnimation(crewCharacters[i], reactions[i % 4], true, 0.6f);
        }
    }

    bool Scene::raiseCrossbow()
    {
        if(crossbowTimer >= 0.0f || isAlbatrossShot())
            return false;

        Character& mariner = characters[marinerCharacter];
        mariner.targetYaw = yawTowards(mariner, toShipLocal(objects[albatrossIndex].position));
        mariner.propVisible = true;
        playAnimation(marinerCharacter, "aim", true, 0.4f);
        crossbowTimer = 0.0f;
        return true;
    }

    glm::vec3 Scene::characterPoint(int character, glm::vec3 localOffset)
    {
        return glm::vec3(characterMatrix(characters[character]) * glm::vec4(localOffset, 1.0f));
    }

    glm::vec3 Scene::characterHead(int character)
    {
        Character& person = characters[character];
        glm::mat4 head = characterMatrix(person) * person.animator.getBoneTransform(person.mesh->findBone("Head"));
        return glm::vec3(head[3]);
    }

    glm::vec3 Scene::shipPoint(glm::vec3 localPoint)
    {
        return toWorld(objects[shipIndex], localPoint);
    }

    int Scene::getMarinerCharacter()
    {
        return marinerCharacter;
    }

    std::vector<int>& Scene::getCrewCharacters()
    {
        return crewCharacters;
    }

    glm::vec3 Scene::getAlbatrossPosition()
    {
        return objects[albatrossIndex].position;
    }

    glm::vec3 Scene::getAlbatrossVelocity()
    {
        return albatrossVelocity;
    }

    float Scene::getAlbatrossArrival()
    {
        return albatrossFollow;
    }

    void Scene::updateCrossbow(float deltaTime)
    {
        if(crossbowTimer < 0.0f)
            return;

        Character& mariner = characters[marinerCharacter];
        float before = crossbowTimer;
        crossbowTimer += deltaTime;

        if(before < CROSSBOW_AIM_TIME)
            mariner.targetYaw = yawTowards(mariner, toShipLocal(objects[albatrossIndex].position));

        if(before < CROSSBOW_AIM_TIME && crossbowTimer >= CROSSBOW_AIM_TIME)
        {
            playAnimation(marinerCharacter, "shoot", false, 0.1f);
            albatrossState = AlbatrossState::Falling;
            albatrossVelocity *= 0.35f;
        }

        if(crossbowTimer >= CROSSBOW_LOWER_TIME)
        {
            mariner.propVisible = false;
            mariner.targetYaw = 0.0f;
            playAnimation(marinerCharacter, "helm", true, 0.8f);
            crossbowTimer = -1.0f;
        }
    }

    void Scene::turnCharacters(float deltaTime)
    {
        for(Character& character : characters)
        {
            float difference = std::remainder(character.targetYaw - character.localYaw, 360.0f);
            float step = CHARACTER_TURN_SPEED * deltaTime;
            character.localYaw += glm::clamp(difference, -step, step);
        }
    }

    void Scene::setSailingWind(float strength)
    {
        sailingWind = strength;
    }

    void Scene::setShipControls(float sail, float helm)
    {
        shipSailInput = sail;
        shipHelmInput = helm;
    }

    void Scene::updateShip(float deltaTime)
    {
        float windStrength = glm::mix(0.7f, 1.0f, stormFactor) * sailingWind;
        shipController.update(deltaTime, shipSailInput, shipHelmInput, windStrength, obstacles);

        SceneObject& playerShip = objects[shipIndex];
        glm::vec2 position = shipController.getPosition();
        playerShip.position.x = position.x;
        playerShip.position.z = position.y;
        playerShip.rotationDegrees.y = shipController.getHeadingDegrees() + SHIP_MODEL_YAW;
        pointLights[shipLanternLight].position = toWorld(playerShip, SHIP_LANTERN);
    }

    glm::vec3 Scene::getShipPosition()
    {
        return objects[shipIndex].position;
    }

    float Scene::getShipHeadingDegrees()
    {
        return shipController.getHeadingDegrees();
    }

    float Scene::getShipSpeed()
    {
        return shipController.getSpeed();
    }

    float Scene::getShipSail()
    {
        return shipController.getSail();
    }

    void Scene::update(float deltaTime, glm::vec3 cameraPosition, glm::vec3 viewDirection)
    {
        float target = weather == Weather::Storm ? 1.0f : 0.0f;
        float step = 0.35f * deltaTime;
        stormFactor += glm::clamp(target - stormFactor, -step, step);
        water.update(deltaTime, stormFactor);
        updateShip(deltaTime);
        updateFloating();
        updateAlbatross(deltaTime);
        updateCrossbow(deltaTime);
        turnCharacters(deltaTime);
        for(Character& character : characters)
            character.animator.update(deltaTime);
        wind.update(deltaTime, stormFactor);
        rain.update(deltaTime, wind.getVelocity(), glm::smoothstep(0.3f, 1.0f, stormFactor));
        lightning.update(deltaTime, stormFactor, cameraPosition, viewDirection);
        applyLightning();
        fog.update(deltaTime, stormFactor, lightning.getFlash());

        beamAngle += BEAM_SPEED_DEGREES * deltaTime;
        if(beamAngle >= 360.0f)
            beamAngle -= 360.0f;

        float angle = glm::radians(beamAngle);
        beam.direction = glm::vec3(std::cos(angle), BEAM_TILT, std::sin(angle));
    }

    void Scene::updateLightSpaces()
    {
        float texel = 2.0f * MOON_SHADOW_RADIUS / MOON_SHADOW_SIZE;
        glm::vec3 shipPosition = getShipPosition();
        glm::vec3 center = glm::vec3(std::floor(shipPosition.x / texel) * texel, 0.0f, std::floor(shipPosition.z / texel) * texel);
        glm::vec3 toMoon = glm::normalize(moon.direction);
        glm::mat4 moonView = glm::lookAt(center + toMoon * MOON_SHADOW_DISTANCE, center, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 moonProjection = glm::ortho(-MOON_SHADOW_RADIUS, MOON_SHADOW_RADIUS, -MOON_SHADOW_RADIUS, MOON_SHADOW_RADIUS, 1.0f, 2.0f * MOON_SHADOW_DISTANCE);
        moonLightSpace = moonProjection * moonView;

        glm::vec3 beamDirection = glm::normalize(beam.direction);
        glm::mat4 beamView = glm::lookAt(beam.position, beam.position + beamDirection, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 beamProjection = glm::perspective(glm::radians(2.0f * beam.outerCutOffDegrees + 4.0f), 1.0f, 0.5f, 250.0f);
        beamLightSpace = beamProjection * beamView;
    }

    void Scene::drawDepth(gps::Shader depthShader, glm::mat4 lightSpace, bool includeLighthouse)
    {
        GLint modelLoc = glGetUniformLocation(depthShader.shaderProgram, "model");
        GLint wingFlapLoc = glGetUniformLocation(depthShader.shaderProgram, "wingFlap");
        glUniformMatrix4fv(glGetUniformLocation(depthShader.shaderProgram, "lightSpace"), 1, GL_FALSE, glm::value_ptr(lightSpace));

        for(SceneObject& object : objects)
        {
            if(!object.visible)
                continue;
            if(!includeLighthouse && object.model == &lighthouse)
                continue;
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(object.getModelMatrix()));
            glUniform1f(wingFlapLoc, object.wingFlap);
            object.model->Draw(depthShader);
        }
    }

    void Scene::renderShadows(gps::Shader depthShader, gps::Shader skinnedDepthShader)
    {
        updateLightSpaces();

        depthShader.useShaderProgram();
        moonShadowMap.begin();
        drawDepth(depthShader, moonLightSpace, true);
        drawCharacterDepth(skinnedDepthShader, moonLightSpace);
        moonShadowMap.end();

        depthShader.useShaderProgram();
        beamShadowMap.begin();
        drawDepth(depthShader, beamLightSpace, false);
        drawCharacterDepth(skinnedDepthShader, beamLightSpace);
        beamShadowMap.end();
    }

    void Scene::uploadEnvironment(gps::Shader shader, glm::vec3 cameraPosition)
    {
        shader.useShaderProgram();
        GLuint program = shader.shaderProgram;
        uploadDirectionalLight(program, "moon", moon);
        uploadPointLights(program, pointLights);
        uploadSpotLight(program, "beam", beam);
        fog.upload(shader);
        glUniform3fv(glGetUniformLocation(program, "cameraPosition"), 1, glm::value_ptr(cameraPosition));
        glUniform1f(glGetUniformLocation(program, "flash"), lightning.getFlash());

        glUniformMatrix4fv(glGetUniformLocation(program, "moonLightSpace"), 1, GL_FALSE, glm::value_ptr(moonLightSpace));
        glUniformMatrix4fv(glGetUniformLocation(program, "beamLightSpace"), 1, GL_FALSE, glm::value_ptr(beamLightSpace));
        glUniform2f(glGetUniformLocation(program, "shadowTexelSize"), 1.0f / moonShadowMap.getSize(), 1.0f / beamShadowMap.getSize());
        moonShadowMap.bind(program, "moonShadowMap", MOON_SHADOW_UNIT);
        beamShadowMap.bind(program, "beamShadowMap", BEAM_SHADOW_UNIT);
    }

    void Scene::drawWater(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition, GLuint skyboxTexture)
    {
        uploadEnvironment(shader, cameraPosition);
        water.draw(shader, view, projection, cameraPosition, skyboxTexture);
    }

    void Scene::drawBeam(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition)
    {
        uploadEnvironment(shader, cameraPosition);
        lightBeam.draw(shader, view, projection, beam.position, beam.direction);
    }

    void Scene::drawWeather(gps::Shader rainShader, gps::Shader boltShader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition)
    {
        uploadEnvironment(boltShader, cameraPosition);
        lightning.draw(boltShader, view, projection, cameraPosition);
        uploadEnvironment(rainShader, cameraPosition);
        rain.draw(rainShader, view, projection);
    }

    SceneObject& Scene::add(gps::Model3D& model, glm::vec3 position, float rotationY, float scale)
    {
        objects.push_back({&model, position, glm::vec3(0.0f, rotationY, 0.0f), scale, 0.0f});
        return objects.back();
    }

    glm::vec3 Scene::onCircle(float angleDegrees, float radius, float height)
    {
        float angle = glm::radians(angleDegrees);
        return glm::vec3(std::sin(angle) * radius, height, std::cos(angle) * radius);
    }

    glm::vec3 Scene::toWorld(SceneObject& object, glm::vec3 localPoint)
    {
        return glm::vec3(object.getModelMatrix() * glm::vec4(localPoint, 1.0f));
    }

    void Scene::draw(gps::Shader shader, GLint modelLoc, GLint normalMatrixLoc, GLint emissiveLoc)
    {
        shader.useShaderProgram();
        GLint wingFlapLoc = glGetUniformLocation(shader.shaderProgram, "wingFlap");

        for(SceneObject& object : objects)
        {
            if(!object.visible)
                continue;

            glm::mat4 model = object.getModelMatrix();
            glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(model));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
            glUniform1f(emissiveLoc, object.emissiveStrength);
            glUniform1f(wingFlapLoc, object.wingFlap);
            object.model->Draw(shader);
        }

        glUniform1f(wingFlapLoc, 0.0f);
        glUniform1f(emissiveLoc, 0.0f);
        for(Character& character : characters)
        {
            if(!character.visible || !character.propVisible || character.prop == nullptr)
                continue;

            glm::mat4 model = characterMatrix(character) * character.animator.getBoneTransform(character.propBone) * character.propOffset;
            glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(model));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
            character.prop->Draw(shader);
        }
    }
}
