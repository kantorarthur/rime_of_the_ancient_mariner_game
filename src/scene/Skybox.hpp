#ifndef Skybox_hpp
#define Skybox_hpp

#include "core/OpenGL.hpp"
#include "graphics/Shader.hpp"

#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace gps {

    class Skybox
    {
    public:
        Skybox() = default;
        ~Skybox();
        Skybox(const Skybox&) = delete;
        Skybox& operator=(const Skybox&) = delete;

        void init();
        int addCubemap(std::string folder);
        void select(int index);
        void draw(gps::Shader shader, glm::mat4 view, glm::mat4 projection, float brightness);
        GLuint getSelectedTexture();

    private:
        GLuint vao = 0;
        GLuint vbo = 0;
        std::vector<GLuint> cubemaps;
        int selected = 0;
    };
}

#endif
