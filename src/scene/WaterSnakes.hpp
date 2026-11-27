#ifndef WaterSnakes_hpp
#define WaterSnakes_hpp

#include "core/OpenGL.hpp"
#include "graphics/Shader.hpp"
#include "scene/Water.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace gps {

    struct WaterSnake
    {
        float angle;
        float radius;
        float speed;
        float phase;
        glm::vec3 color;
        std::vector<glm::vec3> trail;
    };

    class WaterSnakes
    {
    public:
        WaterSnakes() = default;
        ~WaterSnakes();
        WaterSnakes(const WaterSnakes&) = delete;
        WaterSnakes& operator=(const WaterSnakes&) = delete;

        void init(int count);
        void show(bool visible);
        void update(float deltaTime, glm::vec3 center, gps::Water& water);
        void draw(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition);
        glm::vec3 getHeadPosition(int snake);

    private:
        GLuint vao = 0;
        GLuint vbo = 0;
        std::vector<WaterSnake> snakes;
        float time = 0.0f;
        float sampleTimer = 0.0f;
        float intensity = 0.0f;
        float targetIntensity = 0.0f;
    };
}

#endif
