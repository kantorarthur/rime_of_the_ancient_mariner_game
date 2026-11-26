#ifndef Scene_hpp
#define Scene_hpp

#include "game/ShipController.hpp"
#include "graphics/Lights.hpp"
#include "graphics/Model3D.hpp"
#include "graphics/Shader.hpp"
#include "graphics/ShadowMap.hpp"
#include "scene/Character.hpp"
#include "scene/Fog.hpp"
#include "scene/LightBeam.hpp"
#include "scene/Lightning.hpp"
#include "scene/Rain.hpp"
#include "scene/Water.hpp"
#include "scene/Wind.hpp"

#include <glm/glm.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace gps {

    enum class Weather {Calm, Storm};
    enum class AlbatrossState {CirclingLighthouse, FollowingShip, Falling, Floating};

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
        float wingFlap = 0.0f;
        bool visible = true;

        glm::mat4 getModelMatrix();
    };

    class Scene
    {
    public:
        Scene() = default;
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        void load();
        void update(float deltaTime, glm::vec3 cameraPosition, glm::vec3 viewDirection);
        void draw(gps::Shader shader, GLint modelLoc, GLint normalMatrixLoc, GLint emissiveLoc);
        void renderShadows(gps::Shader depthShader, gps::Shader skinnedDepthShader);
        void drawCharacters(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition);
        void uploadEnvironment(gps::Shader shader, glm::vec3 cameraPosition);
        void drawWater(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition, GLuint skyboxTexture);
        void drawBeam(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition);
        void drawWeather(gps::Shader rainShader, gps::Shader boltShader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition);

        void toggleWeather();
        void setWeather(Weather newWeather);
        Weather getWeather();
        void toggleFog();
        bool isFogEnabled();
        void triggerLightning(glm::vec3 cameraPosition, glm::vec3 viewDirection);

        void callAlbatross();
        void crewHailAlbatross();
        bool raiseCrossbow();
        bool isAlbatrossShot();
        void crewMourn();
        void setSailingWind(float strength);

        glm::vec3 characterPoint(int character, glm::vec3 localOffset);
        glm::vec3 characterHead(int character);
        glm::vec3 shipPoint(glm::vec3 localPoint);
        int getMarinerCharacter();
        std::vector<int>& getCrewCharacters();
        glm::vec3 getAlbatrossPosition();
        glm::vec3 getAlbatrossVelocity();
        float getAlbatrossArrival();

        void setShipControls(float sail, float helm);
        glm::vec3 getShipPosition();
        float getShipHeadingDegrees();
        float getShipSpeed();
        float getShipSail();

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
        gps::Model3D albatross;
        gps::Model3D crossbow;
        gps::Model3D rocks[3];

        std::vector<SceneObject> objects;
        std::map<std::string, std::unique_ptr<gps::SkinnedMesh>> skinnedMeshes;
        std::map<std::string, std::unique_ptr<gps::AnimationClip>> clips;
        std::vector<Character> characters;
        std::vector<PointLight> pointLights;
        SpotLight beam;
        DirectionalLight moon;
        DirectionalLight moonBase;

        gps::Water water;
        gps::Fog fog;
        gps::LightBeam lightBeam;
        gps::Wind wind;
        gps::Rain rain;
        gps::Lightning lightning;
        gps::ShadowMap moonShadowMap;
        gps::ShadowMap beamShadowMap;
        glm::mat4 moonLightSpace = glm::mat4(1.0f);
        glm::mat4 beamLightSpace = glm::mat4(1.0f);

        Weather weather = Weather::Calm;
        float stormFactor = 0.0f;
        float beamAngle = 0.0f;

        int shipIndex = -1;
        int shipLanternLight = -1;
        int wreckIndex = -1;
        gps::ShipController shipController;
        std::vector<Obstacle> obstacles;
        float shipSailInput = 0.0f;
        float shipHelmInput = 0.0f;

        int marinerCharacter = -1;
        std::vector<int> crewCharacters;
        float sailingWind = 1.0f;
        float crossbowTimer = -1.0f;

        int albatrossIndex = -1;
        AlbatrossState albatrossState = AlbatrossState::CirclingLighthouse;
        float albatrossFollow = 0.0f;
        glm::vec3 albatrossVelocity = glm::vec3(0.0f);
        float albatrossTime = 0.0f;
        float albatrossAngle = 0.0f;
        glm::vec3 albatrossPrevious = glm::vec3(0.0f);

        void loadModels();
        void placeObjects();
        void placeLights();
        void applyWeather();
        void applyLightning();
        void placeFoam();
        void updateFloating();
        void updateLightSpaces();
        void loadCharacters();
        void placeCrew();
        int addCharacter(std::string meshName, glm::vec3 localPosition, float localYaw, int parentObject);
        void playAnimation(int character, std::string clipName, bool loop, float blendTime);
        glm::mat4 characterMatrix(Character& character);
        void drawCharacterDepth(gps::Shader shader, glm::mat4 lightSpace);
        void updateAlbatross(float deltaTime);
        void updateShip(float deltaTime);
        void placeObstacles();
        glm::vec3 albatrossPathPoint(glm::vec3 center, float radius, float height);
        void flyAlbatross(float deltaTime);
        void dropAlbatross(float deltaTime);
        void updateCrossbow(float deltaTime);
        void turnCharacters(float deltaTime);
        float yawTowards(Character& character, glm::vec3 localTarget);
        glm::vec3 toShipLocal(glm::vec3 worldPoint);
        void drawDepth(gps::Shader depthShader, glm::mat4 lightSpace, bool includeLighthouse);
        void setFloating(SceneObject& object, float draft, float length, float width);
        SceneObject& add(gps::Model3D& model, glm::vec3 position, float rotationY, float scale);
        glm::vec3 onCircle(float angleDegrees, float radius, float height);
        glm::vec3 toWorld(SceneObject& object, glm::vec3 localPoint);
    };
}

#endif
