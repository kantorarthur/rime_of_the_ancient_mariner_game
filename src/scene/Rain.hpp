#ifndef Rain_hpp
#define Rain_hpp

#include "core/OpenGL.hpp"
#include "graphics/Shader.hpp"

#include <glm/glm.hpp>

namespace gps {

    class Rain
    {
    public:
        Rain() = default;
        ~Rain();
        Rain(const Rain&) = delete;
        Rain& operator=(const Rain&) = delete;

        void init(int count);
        void update(float deltaTime, glm::vec3 windVelocity, float intensity);
        void draw(gps::Shader shader, glm::mat4 view, glm::mat4 projection);

    private:
        GLuint vao = 0;
        GLuint vbo = 0;
        int count = 0;

        float intensity = 0.0f;
        glm::vec3 velocity = glm::vec3(0.0f);
        glm::vec3 displacement = glm::vec3(0.0f);
    };
}

#endif
