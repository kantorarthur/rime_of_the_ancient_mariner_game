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
    const float BEAM_SHAFT_LENGTH = 150.0f;
    const float BEAM_SHAFT_RADIUS = 0.6f;
    const float BEAM_SHAFT_SPREAD = 4.5f;

    const int MOON_SHADOW_SIZE = 4096;
    const int BEAM_SHADOW_SIZE = 2048;
    const glm::vec3 MOON_SHADOW_CENTER = glm::vec3(-3.0f, 0.0f, 10.0f);
    const float MOON_SHADOW_RADIUS = 62.0f;
    const float MOON_SHADOW_DISTANCE = 150.0f;
    const GLuint MOON_SHADOW_UNIT = 5;
    const GLuint BEAM_SHADOW_UNIT = 6;

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

        setFloating(add(crate, glm::vec3(-26.0f, 0.0f, 22.0f), 25.0f, 1.0f), 0.22f, 1.17f, 0.53f);
        setFloating(add(crate, glm::vec3(-16.0f, 0.0f, 40.0f), 70.0f, 1.0f), 0.22f, 1.17f, 0.53f);
        setFloating(add(crate, glm::vec3(8.0f, 0.0f, 36.0f), 140.0f, 1.0f), 0.22f, 1.17f, 0.53f);
        setFloating(add(crate, glm::vec3(-38.0f, 0.0f, -4.0f), 200.0f, 1.0f), 0.22f, 1.17f, 0.53f);

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

    void Scene::update(float deltaTime, glm::vec3 cameraPosition, glm::vec3 viewDirection)
    {
        float target = weather == Weather::Storm ? 1.0f : 0.0f;
        float step = 0.35f * deltaTime;
        stormFactor += glm::clamp(target - stormFactor, -step, step);
        water.update(deltaTime, stormFactor);
        updateFloating();
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
        glm::vec3 toMoon = glm::normalize(moon.direction);
        glm::mat4 moonView = glm::lookAt(MOON_SHADOW_CENTER + toMoon * MOON_SHADOW_DISTANCE, MOON_SHADOW_CENTER, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 moonProjection = glm::ortho(-MOON_SHADOW_RADIUS, MOON_SHADOW_RADIUS, -MOON_SHADOW_RADIUS, MOON_SHADOW_RADIUS, 1.0f, 2.0f * MOON_SHADOW_DISTANCE);
        moonLightSpace = moonProjection * moonView;

        glm::vec3 beamDirection = glm::normalize(beam.direction);
        glm::mat4 beamView = glm::lookAt(beam.position, beam.position + beamDirection, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 beamProjection = glm::perspective(glm::radians(2.0f * beam.outerCutOffDegrees + 4.0f), 1.0f, 0.5f, 250.0f);
        beamLightSpace = beamProjection * beamView;
    }

    void Scene::drawDepth(gps::Shader depthShader, gps::ShadowMap& shadowMap, glm::mat4 lightSpace, bool includeLighthouse)
    {
        GLint modelLoc = glGetUniformLocation(depthShader.shaderProgram, "model");
        glUniformMatrix4fv(glGetUniformLocation(depthShader.shaderProgram, "lightSpace"), 1, GL_FALSE, glm::value_ptr(lightSpace));

        shadowMap.begin();
        for(SceneObject& object : objects)
        {
            if(!includeLighthouse && object.model == &lighthouse)
                continue;
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(object.getModelMatrix()));
            object.model->Draw(depthShader);
        }
        shadowMap.end();
    }

    void Scene::renderShadows(gps::Shader depthShader)
    {
        updateLightSpaces();
        depthShader.useShaderProgram();
        drawDepth(depthShader, moonShadowMap, moonLightSpace, true);
        drawDepth(depthShader, beamShadowMap, beamLightSpace, false);
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
