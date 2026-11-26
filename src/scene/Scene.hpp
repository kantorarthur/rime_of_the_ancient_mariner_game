#ifndef Scene_hpp
#define Scene_hpp

#include "graphics/Lights.hpp"
#include "graphics/Model3D.hpp"
#include "graphics/Shader.hpp"
#include "graphics/ShadowMap.hpp"
#include "scene/Fog.hpp"
#include "scene/LightBeam.hpp"
#include "scene/Water.hpp"

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
        float floatDraft = 0.0f;
        float floatLength = 0.0f;
        float floatWidth = 0.0f;

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
        void renderShadows(gps::Shader depthShader);
        void uploadEnvironment(gps::Shader shader, glm::vec3 cameraPosition);
        void drawWater(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition, GLuint skyboxTexture);
        void drawBeam(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition);

        void toggleWeather();
        Weather getWeather();
        void toggleFog();
        bool isFogEnabled();

    private:
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

        gps::Water water;
        gps::Fog fog;
        gps::LightBeam lightBeam;
        gps::ShadowMap moonShadowMap;
        gps::ShadowMap beamShadowMap;
        glm::mat4 moonLightSpace = glm::mat4(1.0f);
        glm::mat4 beamLightSpace = glm::mat4(1.0f);

        Weather weather = Weather::Storm;
        float stormFactor = 1.0f;
        float beamAngle = 0.0f;

        void loadModels();
        void placeObjects();
        void placeLights();
        void applyWeather();
        void placeFoam();
        void updateFloating();
        void updateLightSpaces();
        void drawDepth(gps::Shader depthShader, gps::ShadowMap& shadowMap, glm::mat4 lightSpace, bool includeLighthouse);
        void setFloating(SceneObject& object, float draft, float length, float width);
        SceneObject& add(gps::Model3D& model, glm::vec3 position, float rotationY, float scale);
        glm::vec3 onCircle(float angleDegrees, float radius, float height);
        glm::vec3 toWorld(SceneObject& object, glm::vec3 localPoint);
    };
}

#endif
