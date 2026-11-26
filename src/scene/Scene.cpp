#include "scene/Scene.hpp"

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
        placeLights();
        applyWeather();
    }

    void Scene::loadModels()
    {
        sea.LoadModel("assets/models/sea/sea.obj");
        island.LoadModel("assets/models/island/island.obj");
        cliffRing.LoadModel("assets/models/cliff/cliff_ring.obj");
        lighthouse.LoadModel("assets/models/lighthouse/lighthouse.obj");
        house.LoadModel("assets/models/house/house.obj");
        pier.LoadModel("assets/models/pier/pier_a.obj");
        lantern.LoadModel("assets/models/lantern/lantern.obj");
        boat.LoadModel("assets/models/boat/boat.obj");
        ship.LoadModel("assets/models/ship/ship.obj");
        crate.LoadModel("assets/models/crate/crate.obj");

        for(int i = 0; i < 3; i++)
            rocks[i].LoadModel("assets/models/rocks/rock_" + std::to_string(i + 1) + ".obj");
    }

    void Scene::placeObjects()
    {
        add(sea, glm::vec3(0.0f), 0.0f, 1.0f);
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
        add(boat, boatPosition, PIER_ANGLE + 180.0f, 1.0f);

        SceneObject& wreck = add(ship, glm::vec3(-46.0f, -5.5f, 18.0f), 65.0f, 1.0f);
        wreck.rotationDegrees.x = -7.0f;
        wreck.rotationDegrees.z = 21.0f;

        add(crate, glm::vec3(-26.0f, -0.2f, 22.0f), 25.0f, 1.0f);
        add(crate, glm::vec3(-16.0f, -0.2f, 40.0f), 70.0f, 1.0f);
        add(crate, glm::vec3(8.0f, -0.2f, 36.0f), 140.0f, 1.0f);
        add(crate, glm::vec3(-38.0f, -0.2f, -4.0f), 200.0f, 1.0f);

        add(rocks[0], glm::vec3(-34.0f, -0.8f, 8.0f), 0.0f, 1.6f);
        add(rocks[1], glm::vec3(-42.0f, -1.0f, -10.0f), 50.0f, 2.2f);
        add(rocks[2], glm::vec3(-27.0f, -0.9f, 32.0f), 120.0f, 1.3f);
        add(rocks[0], glm::vec3(31.0f, -0.8f, 27.0f), 200.0f, 1.8f);
        add(rocks[1], glm::vec3(39.0f, -1.0f, -6.0f), 300.0f, 2.0f);
        add(rocks[2], glm::vec3(-10.0f, -0.8f, 44.0f), 80.0f, 1.4f);
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
            moon.direction = CALM_MOON_DIRECTION;
            moon.color = glm::vec3(0.30f, 0.36f, 0.50f);
            moon.ambient = glm::vec3(0.035f, 0.045f, 0.065f);
        }
        else
        {
            moon.direction = STORM_MOON_DIRECTION;
            moon.color = glm::vec3(0.10f, 0.12f, 0.16f);
            moon.ambient = glm::vec3(0.018f, 0.022f, 0.03f);
        }
    }

    void Scene::toggleWeather()
    {
        if(weather == Weather::Calm)
            weather = Weather::Storm;
        else
            weather = Weather::Calm;
        applyWeather();
    }

    Weather Scene::getWeather()
    {
        return weather;
    }

    void Scene::update(float deltaTime)
    {
        beamAngle += BEAM_SPEED_DEGREES * deltaTime;
        if(beamAngle >= 360.0f)
            beamAngle -= 360.0f;

        float angle = glm::radians(beamAngle);
        beam.direction = glm::vec3(std::cos(angle), BEAM_TILT, std::sin(angle));
    }

    void Scene::uploadLights(gps::Shader shader)
    {
        shader.useShaderProgram();
        uploadDirectionalLight(shader.shaderProgram, "moon", moon);
        uploadPointLights(shader.shaderProgram, pointLights);
        uploadSpotLight(shader.shaderProgram, "beam", beam);
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

        for(SceneObject& object : objects)
        {
            glm::mat4 model = object.getModelMatrix();
            glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(model));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
            glUniform1f(emissiveLoc, object.emissiveStrength);
            object.model->Draw(shader);
        }
    }
}
