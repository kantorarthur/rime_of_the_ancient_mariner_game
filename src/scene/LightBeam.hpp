#ifndef LightBeam_hpp
#define LightBeam_hpp

#include "core/OpenGL.hpp"
#include "graphics/Shader.hpp"

#include <glm/glm.hpp>

namespace gps {

    class LightBeam
    {
    public:
        LightBeam() = default;
        ~LightBeam();
        LightBeam(const LightBeam&) = delete;
        LightBeam& operator=(const LightBeam&) = delete;

        void init(float length, float startRadius, float spreadDegrees, int segments);
        void draw(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 apex, glm::vec3 direction);

    private:
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        GLsizei indexCount = 0;

        float length = 0.0f;
        float startRadius = 0.0f;
        float spread = 0.0f;
    };
}

#endif
