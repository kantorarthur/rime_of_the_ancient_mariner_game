#ifndef Scene_hpp
#define Scene_hpp

#include "graphics/Lights.hpp"
#include "graphics/Model3D.hpp"
#include "graphics/Shader.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace gps {

    enum class Weather {Calm, Storm};

    struct SceneObject
    {
        gps::Model3D* model;
        glm::vec3 position;
        glm::vec3 rotationDegrees;
        float scale;
        float emissiveStrength;

        glm::mat4 getModelMatrix();
    };

    class Scene
    {
    public:
        Scene() = default;
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        void load();
        void update(float deltaTime);
        void draw(gps::Shader shader, GLint modelLoc, GLint normalMatrixLoc, GLint emissiveLoc);
        void uploadLights(gps::Shader shader);

        void toggleWeather();
        Weather getWeather();

    private:
        gps::Model3D sea;
        gps::Model3D island;
        gps::Model3D cliffRing;
        gps::Model3D lighthouse;
        gps::Model3D house;
        gps::Model3D pier;
        gps::Model3D lantern;
        gps::Model3D boat;
        gps::Model3D ship;
        gps::Model3D crate;
        gps::Model3D rocks[3];

        std::vector<SceneObject> objects;
        std::vector<PointLight> pointLights;
        SpotLight beam;
        DirectionalLight moon;

        Weather weather = Weather::Storm;
        float beamAngle = 0.0f;

        void loadModels();
        void placeObjects();
        void placeLights();
        void applyWeather();
        SceneObject& add(gps::Model3D& model, glm::vec3 position, float rotationY, float scale);
        glm::vec3 onCircle(float angleDegrees, float radius, float height);
        glm::vec3 toWorld(SceneObject& object, glm::vec3 localPoint);
    };
}

#endif
