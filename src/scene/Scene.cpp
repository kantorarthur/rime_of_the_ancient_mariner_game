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

    glm::mat4 SceneObject::getModelMatrix()
    {
        glm::mat4 matrix = glm::translate(glm::mat4(1.0f), position);
        matrix = glm::rotate(matrix, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::scale(matrix, glm::vec3(scale));
        return matrix;
    }

    void Scene::load()
    {
        loadModels();
        placeObjects();
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

        add(lighthouse, glm::vec3(6.0f, 3.3f, -6.0f), 0.0f, 1.0f);
        add(house, glm::vec3(-5.0f, 3.45f, -1.0f), 320.0f, 1.0f);

        float deckY = PIER_DECK_HEIGHT - PIER_MODEL_DECK;
        for(int i = 0; i < 4; i++)
            add(pier, onCircle(PIER_ANGLE, 21.5f + i * PIER_SECTION_LENGTH, deckY), PIER_ANGLE, 1.0f);

        glm::vec3 pierEnd = onCircle(PIER_ANGLE, 21.5f + 3.0f * PIER_SECTION_LENGTH + 1.0f, PIER_DECK_HEIGHT);
        glm::vec3 pierSide = glm::vec3(std::cos(glm::radians(PIER_ANGLE)), 0.0f, -std::sin(glm::radians(PIER_ANGLE)));
        add(lantern, pierEnd + pierSide * 0.9f, 0.0f, 1.0f);

        glm::vec3 boatPosition = onCircle(PIER_ANGLE, 28.0f, -0.3f) - pierSide * 3.0f;
        add(boat, boatPosition, PIER_ANGLE + 180.0f, 1.0f);

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

    void Scene::add(gps::Model3D& model, glm::vec3 position, float rotationY, float scale)
    {
        objects.push_back({&model, position, rotationY, scale});
    }

    glm::vec3 Scene::onCircle(float angleDegrees, float radius, float height)
    {
        float angle = glm::radians(angleDegrees);
        return glm::vec3(std::sin(angle) * radius, height, std::cos(angle) * radius);
    }

    void Scene::draw(gps::Shader shader, glm::mat4 view, GLint modelLoc, GLint normalMatrixLoc)
    {
        shader.useShaderProgram();

        for(SceneObject& object : objects)
        {
            glm::mat4 model = object.getModelMatrix();
            glm::mat3 normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
            object.model->Draw(shader);
        }
    }
}
