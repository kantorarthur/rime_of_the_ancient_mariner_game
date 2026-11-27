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
    const glm::mat4 HUNG_ALBATROSS = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.24f, -0.02f)), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(0.38f));
    const float GHOST_DECK = 4.65f;
    const glm::vec3 GHOST_LIGHT = glm::vec3(0.0f, 7.8f, 0.6f);
    const glm::vec3 GHOST_LIGHT_COLOR = glm::vec3(0.55f, 0.8f, 0.9f) * 2.2f;
    const float GHOST_APPROACH_TIME = 26.0f;
    const float GHOST_LEAVE_TIME = 7.0f;
    const float GHOST_DISTANCE = 40.0f;
    const float SEAT_HEIGHT = 0.464f;
    const float DICE_HALF_SIZE = 0.035f;
    const float DICE_FLIGHT_TIME = 0.8f;
    const float SOUL_DELAY = 1.3f;
    const float SOUL_LIFETIME = 6.0f;
    const int WATER_SNAKE_COUNT = 12;
    const float SINK_TIME = 9.0f;
    const glm::vec3 SUNK_OFFSET = glm::vec3(-14.0f, -34.0f, 24.0f);
    const float SWIM_DEPTH = -0.15f;
    const float BOAT_SPEED = 3.2f;
    const float BOAT_FLOOR = 0.12f;
    const glm::mat4 CROSSBOW_GRIP = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.06f));

    const std::string CHARACTER_FOLDER = "assets/characters/";
    const char* CHARACTER_MESHES[] = {"mariner", "sailor_1", "sailor_2", "sailor_3", "sailor_4", "death", "life_in_death", "pilot", "pilot_boy", "hermit"};
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

    glm::mat4 Scene::objectMatrix(SceneObject& object)
    {
        if(object.parent >= 0)
            return objectMatrix(objects[object.parent]) * object.getModelMatrix();
        return object.getModelMatrix();
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
        waterSnakes.init(WATER_SNAKE_COUNT);

        glGenVertexArrays(1, &soulVao);
        glGenBuffers(1, &soulVbo);
        glBindVertexArray(soulVao);
        glBindBuffer(GL_ARRAY_BUFFER, soulVbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
        glBindVertexArray(0);
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
        dice.LoadModel("assets/models/dice/dice.obj");

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
        boatIndex = (int)objects.size() - 1;
        boatHome = boatPosition;

        SceneObject& wreck = add(ship, glm::vec3(-46.0f, -5.5f, 18.0f), 65.0f, 1.0f);
        wreck.rotationDegrees.x = -7.0f;
        wreck.rotationDegrees.z = 21.0f;
        wreck.visible = false;
        wreckIndex = (int)objects.size() - 1;

        shipController.reset(SHIP_START_POSITION, SHIP_START_HEADING);
        setFloating(add(ship, glm::vec3(SHIP_START_POSITION.x, 0.0f, SHIP_START_POSITION.y), SHIP_START_HEADING + SHIP_MODEL_YAW, 1.0f), SHIP_DRAFT, SHIP_LENGTH, SHIP_WIDTH);
        shipIndex = (int)objects.size() - 1;

        placeGhostShip();

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
        Prop bow;
        bow.model = &crossbow;
        bow.bone = mariner.mesh->findBone("hand_r");
        bow.offset = CROSSBOW_GRIP;
        mariner.props.push_back(bow);

        Prop deadBird;
        deadBird.model = &albatross;
        deadBird.bone = mariner.mesh->findBone("spine_03");
        deadBird.offset = HUNG_ALBATROSS;
        deadBird.wingFlap = glm::radians(-70.0f);
        mariner.props.push_back(deadBird);

        crewCharacters.push_back(addCharacter("sailor_1", glm::vec3(2.0f, 4.65f, 1.0f), 90.0f, shipIndex));
        crewCharacters.push_back(addCharacter("sailor_2", glm::vec3(-2.0f, 4.65f, -1.0f), -90.0f, shipIndex));
        crewCharacters.push_back(addCharacter("sailor_3", glm::vec3(0.8f, 6.3f, 6.0f), 0.0f, shipIndex));
        crewCharacters.push_back(addCharacter("sailor_4", glm::vec3(-1.0f, 4.65f, 2.5f), 90.0f, shipIndex));

        const char* work[] = {"rail", "rail_call", "fold_arms", "haul"};
        for(size_t i = 0; i < crewCharacters.size(); i++)
        {
            playAnimation(crewCharacters[i], work[i], true, 0.0f);
            crewWorkYaw.push_back(characters[crewCharacters[i]].localYaw);
        }

        pilotCharacter = addCharacter("pilot", glm::vec3(0.0f, BOAT_FLOOR, 0.3f), 0.0f, boatIndex);
        pilotBoyCharacter = addCharacter("pilot_boy", glm::vec3(0.0f, BOAT_FLOOR, 1.4f), 180.0f, boatIndex);
        hermitCharacter = addCharacter("hermit", glm::vec3(0.0f, BOAT_FLOOR, -1.0f), 0.0f, boatIndex);
        characters[pilotBoyCharacter].scale = 0.85f;
        for(int person : {pilotCharacter, pilotBoyCharacter, hermitCharacter})
        {
            playAnimation(person, "sit", true, 0.0f);
            characters[person].visible = false;
        }

        deathCharacter = addCharacter("death", glm::vec3(-0.95f, GHOST_DECK, 0.0f), 90.0f, ghostIndex);
        playAnimation(deathCharacter, "sit", true, 0.0f);
        characters[deathCharacter].visible = false;

        lifeInDeathCharacter = addCharacter("life_in_death", glm::vec3(0.95f, GHOST_DECK, 0.0f), -90.0f, ghostIndex);
        playAnimation(lifeInDeathCharacter, "sit_talk", true, 0.0f);
        characters[lifeInDeathCharacter].visible = false;
    }

    glm::mat4 Scene::characterMatrix(Character& character)
    {
        glm::mat4 parent = glm::mat4(1.0f);
        if(character.parentObject >= 0)
            parent = objectMatrix(objects[character.parentObject]);
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

    void Scene::placeGhostShip()
    {
        SceneObject& spectre = add(ship, glm::vec3(0.0f, -SHIP_DRAFT, 0.0f), 0.0f, 1.0f);
        spectre.ghost = true;
        spectre.visible = false;
        ghostIndex = (int)objects.size() - 1;

        glm::vec3 furniture[3] = {glm::vec3(-1.23f, GHOST_DECK, 0.0f), glm::vec3(1.23f, GHOST_DECK, 0.0f), glm::vec3(0.0f, GHOST_DECK, 0.0f)};
        float furnitureYaw[3] = {90.0f, 90.0f, 0.0f};
        for(int i = 0; i < 3; i++)
        {
            SceneObject& seat = add(crate, furniture[i], furnitureYaw[i], 1.0f);
            seat.parent = ghostIndex;
            seat.ghost = true;
            seat.visible = false;
        }

        for(int i = 0; i < 2; i++)
        {
            SceneObject& die = add(dice, glm::vec3(0.0f, GHOST_DECK + SEAT_HEIGHT + DICE_HALF_SIZE, 0.0f), 0.0f, 1.0f);
            die.parent = ghostIndex;
            die.visible = false;
            diceIndex[i] = (int)objects.size() - 1;
        }
    }

    void Scene::lightDeathFires()
    {
        curseTarget = 1.0f;
    }

    void Scene::hangAlbatross()
    {
        albatrossState = AlbatrossState::Hung;
        objects[albatrossIndex].visible = false;
        characters[marinerCharacter].props[1].visible = true;
    }

    void Scene::marinerBitesArm()
    {
        playAnimation(marinerCharacter, "consume", false, 0.4f);
        characters[marinerCharacter].nextClip = "helm";
        for(int sailor : crewCharacters)
            playAnimation(sailor, "rail_call", true, 0.6f);
    }

    void Scene::summonGhostShip()
    {
        if(ghostState != GhostState::Hidden)
            return;

        glm::vec3 forward = shipPoint(glm::vec3(0.0f, 0.0f, 1.0f)) - shipPoint(glm::vec3(0.0f));
        forward = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));
        glm::vec3 right = glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f));

        glm::vec3 alongside = shipPoint(glm::vec3(-GHOST_DISTANCE, 0.0f, 0.0f));
        ghostEnd = glm::vec3(alongside.x, -SHIP_DRAFT, alongside.z);
        ghostStart = ghostEnd + forward * 380.0f - right * 140.0f;
        ghostYaw = objects[shipIndex].rotationDegrees.y;

        ghostState = GhostState::Approaching;
        ghostTime = 0.0f;
        for(SceneObject& object : objects)
        {
            if(object.parent == ghostIndex && object.ghost)
                object.visible = true;
        }
        objects[ghostIndex].visible = true;
        characters[deathCharacter].visible = true;
        characters[lifeInDeathCharacter].visible = true;
    }

    bool Scene::isGhostShipAlongside()
    {
        return ghostState == GhostState::Alongside;
    }

    void Scene::dismissGhostShip()
    {
        if(ghostState == GhostState::Alongside || ghostState == GhostState::Approaching)
        {
            ghostState = GhostState::Leaving;
            ghostTime = 0.0f;
            ghostStart = objects[ghostIndex].position;
        }
    }

    void Scene::updateGhostShip(float deltaTime)
    {
        if(ghostState == GhostState::Hidden)
            return;

        ghostTime += deltaTime;
        SceneObject& spectre = objects[ghostIndex];
        spectre.rotationDegrees = glm::vec3(0.0f, ghostYaw, 0.0f);

        float yaw = glm::radians(ghostYaw);
        glm::vec3 bow = glm::vec3(std::sin(yaw), 0.0f, std::cos(yaw));
        float bob = 0.15f * std::sin(ghostTime * 0.7f);

        if(ghostState == GhostState::Approaching)
        {
            float progress = glm::clamp(ghostTime / GHOST_APPROACH_TIME, 0.0f, 1.0f);
            spectre.position = glm::mix(ghostStart, ghostEnd, glm::smoothstep(0.0f, 1.0f, progress));
            ghostFade = glm::smoothstep(0.0f, 0.35f, progress);
            if(progress >= 1.0f)
                ghostState = GhostState::Alongside;
        }
        else if(ghostState == GhostState::Alongside)
        {
            spectre.position = ghostEnd;
            ghostFade = 1.0f;
        }
        else
        {
            float progress = glm::clamp(ghostTime / GHOST_LEAVE_TIME, 0.0f, 1.0f);
            spectre.position = ghostStart + bow * (progress * progress * 420.0f);
            ghostFade = 1.0f - glm::smoothstep(0.35f, 1.0f, progress);
            if(progress >= 1.0f)
            {
                ghostState = GhostState::Hidden;
                for(SceneObject& object : objects)
                {
                    if(object.parent == ghostIndex)
                        object.visible = false;
                }
                spectre.visible = false;
                pointLights[ghostLight].color = glm::vec3(0.0f);
                characters[deathCharacter].visible = false;
                characters[lifeInDeathCharacter].visible = false;
            }
        }
        spectre.position.y += bob;
        pointLights[ghostLight].position = toWorld(spectre, GHOST_LIGHT);
        pointLights[ghostLight].color = GHOST_LIGHT_COLOR * ghostFade;
    }

    void Scene::throwDice()
    {
        diceTime = 0.0f;
        objects[diceIndex[0]].visible = true;
        objects[diceIndex[1]].visible = true;
    }

    void Scene::updateDice(float deltaTime)
    {
        if(diceTime < 0.0f)
            return;

        diceTime += deltaTime;
        float progress = glm::clamp(diceTime / DICE_FLIGHT_TIME, 0.0f, 1.0f);
        float table = GHOST_DECK + SEAT_HEIGHT + DICE_HALF_SIZE;

        for(int i = 0; i < 2; i++)
        {
            SceneObject& die = objects[diceIndex[i]];
            glm::vec3 start = glm::vec3(0.55f, GHOST_DECK + 1.0f, 0.1f - 0.2f * i);
            glm::vec3 end = glm::vec3(-0.08f + 0.17f * i, table, 0.12f - 0.22f * i);
            die.position = glm::mix(start, end, progress);
            die.position.y += 0.35f * std::sin(progress * glm::pi<float>()) * (1.0f - progress);

            float spin = (1.0f - progress) * (1.0f - progress);
            die.rotationDegrees = glm::vec3(720.0f * spin + 90.0f * i, 35.0f + 60.0f * i + 540.0f * spin, 540.0f * spin);
        }
    }

    void Scene::lifeInDeathWins()
    {
        playAnimation(lifeInDeathCharacter, "yes", false, 0.4f);
        characters[lifeInDeathCharacter].nextClip = "idle";
    }

    void Scene::crewCurse()
    {
        Character& mariner = characters[marinerCharacter];
        for(int sailor : crewCharacters)
        {
            characters[sailor].targetYaw = yawTowards(characters[sailor], mariner.localPosition);
            playAnimation(sailor, "idle", true, 0.6f);
        }
    }

    void Scene::killCrew(float interval)
    {
        crewDeathInterval = interval;
        crewDeathTimer = 0.0f;
        crewDeaths = 0;
    }

    void Scene::updateCrewDeaths(float deltaTime)
    {
        if(crewDeathTimer < 0.0f)
            return;

        crewDeathTimer += deltaTime;
        while(crewDeaths < (int)crewCharacters.size() && crewDeathTimer >= crewDeaths * crewDeathInterval)
        {
            int sailor = crewCharacters[crewDeaths];
            playAnimation(sailor, "death", false, 0.2f);
            glm::vec3 chest = characterHead(sailor) - glm::vec3(0.0f, 0.4f, 0.0f);
            souls.push_back({chest, glm::vec3(0.0f), -SOUL_DELAY});
            crewDeaths++;
        }
        if(crewDeaths >= (int)crewCharacters.size())
            crewDeathTimer = -1.0f;
    }

    void Scene::updateSouls(float deltaTime)
    {
        glm::vec3 marinerHead = characterHead(marinerCharacter);
        for(Soul& soul : souls)
        {
            soul.age += deltaTime;
            if(soul.age < 0.0f)
                continue;

            glm::vec3 desired;
            if(soul.age < 1.2f)
                desired = glm::vec3(0.0f, 1.6f, 0.0f);
            else if(soul.age < 2.6f)
                desired = glm::normalize(marinerHead + glm::vec3(0.0f, 0.3f, 0.0f) - soul.position) * 8.0f;
            else
                desired = glm::normalize(soul.velocity + glm::vec3(0.0f, 0.8f, 0.0f)) * 14.0f;

            soul.velocity = glm::mix(soul.velocity, desired, glm::min(1.0f, 3.0f * deltaTime));
            soul.position += soul.velocity * deltaTime;
        }

        std::vector<Soul> alive;
        for(Soul& soul : souls)
        {
            if(soul.age < SOUL_LIFETIME)
                alive.push_back(soul);
        }
        souls = alive;
    }

    void Scene::marinerDespairs()
    {
        playAnimation(marinerCharacter, "grieve", true, 0.8f);
    }

    int Scene::getDeathCharacter()
    {
        return deathCharacter;
    }

    int Scene::getLifeInDeathCharacter()
    {
        return lifeInDeathCharacter;
    }

    glm::vec3 Scene::getGhostShipPoint(glm::vec3 localPoint)
    {
        return toWorld(objects[ghostIndex], localPoint);
    }

    glm::vec3 Scene::getDicePosition()
    {
        return toWorld(objects[diceIndex[0]], glm::vec3(0.0f));
    }

    void Scene::drawProps(gps::Shader shader, GLint modelLoc, GLint normalMatrixLoc)
    {
        GLint wingFlapLoc = glGetUniformLocation(shader.shaderProgram, "wingFlap");
        for(Character& character : characters)
        {
            if(!character.visible)
                continue;

            for(Prop& prop : character.props)
            {
                if(!prop.visible)
                    continue;

                glm::mat4 model = characterMatrix(character) * character.animator.getBoneTransform(prop.bone) * prop.offset;
                glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(model));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
                glUniform1f(wingFlapLoc, prop.wingFlap);
                prop.model->Draw(shader);
            }
        }
    }

    void Scene::drawGhosts(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition)
    {
        if(ghostState == GhostState::Hidden)
            return;

        uploadEnvironment(shader, cameraPosition);
        GLuint program = shader.shaderProgram;
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1f(glGetUniformLocation(program, "wingFlap"), 0.0f);
        glUniform1f(glGetUniformLocation(program, "ghostFade"), ghostFade);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glDepthMask(GL_FALSE);

        for(SceneObject& object : objects)
        {
            if(!object.visible || !object.ghost)
                continue;

            glm::mat4 model = objectMatrix(object);
            glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(model));
            glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix3fv(glGetUniformLocation(program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
            object.model->Draw(shader);
        }

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    void Scene::drawSouls(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition)
    {
        std::vector<glm::vec4> points;
        for(Soul& soul : souls)
        {
            if(soul.age < 0.0f)
                continue;
            float alpha = glm::smoothstep(0.0f, 0.5f, soul.age) * (1.0f - glm::smoothstep(SOUL_LIFETIME - 1.5f, SOUL_LIFETIME, soul.age));
            points.push_back(glm::vec4(soul.position, alpha));
        }
        if(seraphFade > 0.0f)
        {
            for(int sailor : crewCharacters)
            {
                glm::vec3 above = characterPoint(sailor, glm::vec3(0.0f, 1.6f, 0.0f));
                above.y += 0.15f * std::sin(albatrossTime * 2.0f + sailor);
                points.push_back(glm::vec4(above, seraphFade * 0.8f));
            }
        }
        if(points.empty())
            return;

        uploadEnvironment(shader, cameraPosition);
        GLuint program = shader.shaderProgram;
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glDepthMask(GL_FALSE);

        glBindVertexArray(soulVao);
        glBindBuffer(GL_ARRAY_BUFFER, soulVbo);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec4), points.data(), GL_STREAM_DRAW);
        glDrawArrays(GL_POINTS, 0, (GLsizei)points.size());
        glBindVertexArray(0);

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glDisable(GL_PROGRAM_POINT_SIZE);
    }

    void Scene::drawSnakes(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition)
    {
        uploadEnvironment(shader, cameraPosition);
        waterSnakes.draw(shader, view, projection, cameraPosition);
    }

    void Scene::showWaterSnakes(bool visible)
    {
        waterSnakes.show(visible);
    }

    void Scene::marinerBlesses()
    {
        playAnimation(marinerCharacter, "bless", false, 0.3f);
        characters[marinerCharacter].nextClip = "bless_hold";
    }

    void Scene::liftCurse()
    {
        curseTarget = 0.0f;
    }

    void Scene::dropHungAlbatross()
    {
        Character& mariner = characters[marinerCharacter];
        Prop& deadBird = mariner.props[1];
        glm::mat4 hung = characterMatrix(mariner) * mariner.animator.getBoneTransform(deadBird.bone) * deadBird.offset;
        deadBird.visible = false;

        SceneObject& bird = objects[albatrossIndex];
        bird.position = glm::vec3(hung[3]);
        bird.rotationDegrees = glm::vec3(-80.0f, objects[shipIndex].rotationDegrees.y, 0.0f);
        bird.scale = 0.5f;
        bird.wingFlap = glm::radians(-40.0f);
        bird.visible = true;
        glm::vec3 overboard = shipPoint(glm::vec3(-1.0f, 0.0f, 0.0f)) - shipPoint(glm::vec3(0.0f));
        albatrossVelocity = glm::normalize(glm::vec3(overboard.x, 0.0f, overboard.z)) * 4.5f + glm::vec3(0.0f, 2.5f, 0.0f);
        albatrossState = AlbatrossState::Sinking;
        playAnimation(marinerCharacter, "kneel", true, 0.6f);
        mariner.nextClip.clear();
    }

    void Scene::crewRise()
    {
        const char* work[] = {"haul", "zombie", "haul", "zombie"};
        for(size_t i = 0; i < crewCharacters.size(); i++)
        {
            Character& sailor = characters[crewCharacters[i]];
            sailor.targetYaw = crewWorkYaw[i];
            playAnimation(crewCharacters[i], "rise", false, 0.3f);
            sailor.nextClip = work[i % 4];
        }
        playAnimation(marinerCharacter, "helm", true, 0.8f);
    }

    void Scene::crewFallAgain()
    {
        for(int sailor : crewCharacters)
        {
            characters[sailor].nextClip.clear();
            playAnimation(sailor, "death", false, 0.3f);
        }
        seraphTarget = 1.0f;
    }

    float Scene::getSailingWind()
    {
        return sailingWind;
    }

    void Scene::sinkShip()
    {
        if(shipSinking)
            return;

        SceneObject& playerShip = objects[shipIndex];
        shipSinking = true;
        sinkTime = 0.0f;
        sinkStartPosition = playerShip.position;
        sinkStartRotation = playerShip.rotationDegrees;
        playerShip.floatLength = 0.0f;
        pointLights[shipLanternLight].color = glm::vec3(0.0f);
        water.addFoamCircle(glm::vec2(playerShip.position.x, playerShip.position.z), 9.0f, 4.0f);
        seraphTarget = 0.0f;
    }

    void Scene::updateSinking(float deltaTime)
    {
        if(!shipSinking || sinkTime >= SINK_TIME)
            return;

        sinkTime += deltaTime;
        float progress = glm::clamp(sinkTime / SINK_TIME, 0.0f, 1.0f);
        float eased = std::pow(progress, 1.2f);

        SceneObject& playerShip = objects[shipIndex];
        playerShip.position = sinkStartPosition + glm::vec3(0.0f, SUNK_OFFSET.y * eased, 0.0f);
        playerShip.rotationDegrees.x = glm::mix(sinkStartRotation.x, SUNK_OFFSET.x, eased);
        playerShip.rotationDegrees.z = glm::mix(sinkStartRotation.z, SUNK_OFFSET.z, eased);
        playerShip.rotationDegrees.x += 1.5f * std::sin(sinkTime * 2.3f) * (1.0f - progress);

        if(progress >= 1.0f)
        {
            for(int sailor : crewCharacters)
                characters[sailor].visible = false;
        }
    }

    void Scene::marinerAfloat()
    {
        Character& mariner = characters[marinerCharacter];
        glm::vec3 position = shipPoint(glm::vec3(-7.0f, 0.0f, -15.0f));
        mariner.parentObject = -1;
        mariner.localPosition = position;
        mariner.localYaw = objects[shipIndex].rotationDegrees.y;
        mariner.targetYaw = mariner.localYaw;
        mariner.props[0].visible = false;
        mariner.props[1].visible = false;
        mariner.nextClip.clear();
        playAnimation(marinerCharacter, "swim", true, 0.3f);
        marinerSwimming = true;

        glm::vec3 toBoat = objects[boatIndex].position - position;
        toBoat.y = 0.0f;
        boatTarget = position + glm::normalize(toBoat) * 2.6f;
        boatState = BoatState::Rowing;
    }

    bool Scene::isBoatHome()
    {
        return boatReturned;
    }

    bool Scene::isShipSinking()
    {
        return shipSinking;
    }

    void Scene::marinerAshore()
    {
        Character& mariner = characters[marinerCharacter];
        mariner.parentObject = -1;
        mariner.localPosition = onCircle(PIER_ANGLE, 27.0f, PIER_DECK_HEIGHT);
        mariner.localYaw = 90.0f - PIER_ANGLE;
        mariner.targetYaw = mariner.localYaw;
        playAnimation(marinerCharacter, "idle", true, 0.5f);
        playAnimation(hermitCharacter, "bless", true, 0.5f);
    }

    void Scene::launchBoat()
    {
        glm::vec3 ship = shipPoint(glm::vec3(0.0f));
        glm::vec3 toHome = boatHome - ship;
        toHome.y = 0.0f;
        boatTarget = ship + glm::normalize(toHome) * 24.0f;
        boatState = BoatState::Rowing;
        for(int person : {pilotCharacter, pilotBoyCharacter, hermitCharacter})
            characters[person].visible = true;
        playAnimation(pilotCharacter, "sit_talk", true, 0.3f);
    }

    void Scene::updateSwimming()
    {
        if(!marinerSwimming)
            return;
        Character& mariner = characters[marinerCharacter];
        mariner.localPosition.y = water.heightAt(mariner.localPosition.x, mariner.localPosition.z) - SWIM_DEPTH;
    }

    void Scene::updateBoat(float deltaTime)
    {
        if(boatState != BoatState::Rowing && boatState != BoatState::Returning)
            return;

        SceneObject& rescue = objects[boatIndex];
        glm::vec3 target = boatState == BoatState::Rowing ? boatTarget : boatHome;
        glm::vec3 offset = glm::vec3(target.x - rescue.position.x, 0.0f, target.z - rescue.position.z);
        float distance = glm::length(offset);
        if(distance < 0.3f)
        {
            if(boatState == BoatState::Returning)
                boatReturned = true;
            boatState = boatState == BoatState::Rowing ? BoatState::Waiting : BoatState::Moored;
            return;
        }

        glm::vec3 direction = offset / distance;
        float speed = BOAT_SPEED * glm::clamp(distance / 4.0f, 0.25f, 1.0f);
        rescue.position += direction * speed * deltaTime;
        float heading = glm::degrees(std::atan2(direction.x, direction.z));
        float turn = std::remainder(heading - rescue.rotationDegrees.y, 360.0f);
        rescue.rotationDegrees.y += glm::clamp(turn, -40.0f * deltaTime, 40.0f * deltaTime);
    }

    bool Scene::isBoatAtMariner()
    {
        return boatState == BoatState::Waiting;
    }

    void Scene::marinerAboard()
    {
        Character& mariner = characters[marinerCharacter];
        marinerSwimming = false;
        mariner.parentObject = boatIndex;
        mariner.localPosition = glm::vec3(0.0f, BOAT_FLOOR, -1.9f);
        mariner.localYaw = 0.0f;
        mariner.targetYaw = 0.0f;
        playAnimation(marinerCharacter, "sit", true, 0.5f);
    }

    void Scene::pilotFit()
    {
        playAnimation(pilotCharacter, "death", false, 0.3f);
        playAnimation(hermitCharacter, "kneel", true, 0.5f);
    }

    void Scene::boyLaughs()
    {
        playAnimation(pilotBoyCharacter, "yes", true, 0.3f);
        boatState = BoatState::Returning;
    }

    int Scene::getPilotCharacter()
    {
        return pilotCharacter;
    }

    int Scene::getPilotBoyCharacter()
    {
        return pilotBoyCharacter;
    }

    int Scene::getHermitCharacter()
    {
        return hermitCharacter;
    }

    glm::vec3 Scene::getSnakeHead(int snake)
    {
        return waterSnakes.getHeadPosition(snake);
    }

    glm::vec3 Scene::getBoatPoint(glm::vec3 localPoint)
    {
        return toWorld(objects[boatIndex], localPoint);
    }

    glm::vec3 Scene::getLighthouseTop()
    {
        return LIGHTHOUSE_POSITION + glm::vec3(0.0f, LIGHTHOUSE_LAMP_HEIGHT, 0.0f);
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

        pointLights.push_back({glm::vec3(0.0f, -100.0f, 0.0f), glm::vec3(0.0f), 1.0f, 0.2f, 0.08f});
        ghostLight = (int)pointLights.size() - 1;

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
        if(albatrossState == AlbatrossState::Hung || albatrossState == AlbatrossState::Gone)
            return;
        if(albatrossState == AlbatrossState::Sinking)
        {
            SceneObject& bird = objects[albatrossIndex];
            float surface = water.heightAt(bird.position.x, bird.position.z);
            if(bird.position.y > surface)
                albatrossVelocity.y -= 9.81f * deltaTime;
            else
                albatrossVelocity = glm::vec3(0.0f, -0.6f, 0.0f);
            bird.position += albatrossVelocity * deltaTime;
            bird.rotationDegrees.x -= 40.0f * deltaTime;
            if(bird.position.y < surface - 3.0f)
            {
                bird.visible = false;
                albatrossState = AlbatrossState::Gone;
            }
            return;
        }
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
        return glm::vec3(glm::inverse(objectMatrix(objects[shipIndex])) * glm::vec4(worldPoint, 1.0f));
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
        mariner.props[0].visible = true;
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
            mariner.props[0].visible = false;
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
        if(shipSinking)
            return;

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
        updateGhostShip(deltaTime);
        updateDice(deltaTime);
        updateCrewDeaths(deltaTime);
        turnCharacters(deltaTime);
        for(Character& character : characters)
        {
            if(!character.nextClip.empty() && character.animator.isFinished())
            {
                character.animator.play(clips[character.nextClip].get(), true, 0.5f);
                character.nextClip.clear();
            }
            character.animator.update(deltaTime);
        }
        updateSouls(deltaTime);
        updateSinking(deltaTime);
        updateBoat(deltaTime);
        updateSwimming();
        waterSnakes.update(deltaTime, getShipPosition(), water);

        float seraphStep = 0.5f * deltaTime;
        seraphFade += glm::clamp(seraphTarget - seraphFade, -seraphStep, seraphStep);

        float curseStep = 0.15f * deltaTime;
        curse += glm::clamp(curseTarget - curse, -curseStep, curseStep);
        water.setCurse(curse);
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
            if(!object.visible || object.ghost)
                continue;
            if(!includeLighthouse && object.model == &lighthouse)
                continue;
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(objectMatrix(object)));
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
        return glm::vec3(objectMatrix(object) * glm::vec4(localPoint, 1.0f));
    }

    void Scene::draw(gps::Shader shader, GLint modelLoc, GLint normalMatrixLoc, GLint emissiveLoc)
    {
        shader.useShaderProgram();
        GLint wingFlapLoc = glGetUniformLocation(shader.shaderProgram, "wingFlap");

        for(SceneObject& object : objects)
        {
            if(!object.visible || object.ghost)
                continue;

            glm::mat4 model = objectMatrix(object);
            glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(model));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
            glUniform1f(emissiveLoc, object.emissiveStrength);
            glUniform1f(wingFlapLoc, object.wingFlap);
            object.model->Draw(shader);
        }

        glUniform1f(emissiveLoc, 0.0f);
        drawProps(shader, modelLoc, normalMatrixLoc);
        glUniform1f(wingFlapLoc, 0.0f);
    }
}
