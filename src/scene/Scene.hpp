#ifndef Scene_hpp
#define Scene_hpp

#include "graphics/Model3D.hpp"
#include "graphics/Shader.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace gps {

    struct SceneObject
    {
        gps::Model3D* model;
        glm::vec3 position;
        float rotationY;
        float scale;

        glm::mat4 getModelMatrix();
    };

    class Scene
    {
    public:
        Scene() = default;
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        void load();
        void draw(gps::Shader shader, glm::mat4 view, GLint modelLoc, GLint normalMatrixLoc);

    private:
        gps::Model3D sea;
        gps::Model3D island;
        gps::Model3D cliffRing;
        gps::Model3D lighthouse;
        gps::Model3D house;
        gps::Model3D pier;
        gps::Model3D lantern;
        gps::Model3D boat;
        gps::Model3D crate;
        gps::Model3D rocks[3];

        std::vector<SceneObject> objects;

        void loadModels();
        void placeObjects();
        void add(gps::Model3D& model, glm::vec3 position, float rotationY, float scale);
        glm::vec3 onCircle(float angleDegrees, float radius, float height);
    };
}

#endif
