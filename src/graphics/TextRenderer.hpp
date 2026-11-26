#ifndef TextRenderer_hpp
#define TextRenderer_hpp

#include "core/OpenGL.hpp"
#include "graphics/Shader.hpp"

#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace gps {

    class TextRenderer
    {
    public:
        TextRenderer() = default;
        ~TextRenderer();
        TextRenderer(const TextRenderer&) = delete;
        TextRenderer& operator=(const TextRenderer&) = delete;

        void init(gps::Shader shader);
        void begin(int screenWidth, int screenHeight);
        void end();

        void drawText(std::string text, float x, float y, float scale, glm::vec4 color);
        void drawTextCentered(std::string text, float centerX, float y, float scale, glm::vec4 color);
        void drawRectangle(float x, float y, float width, float height, glm::vec4 color);

        float measure(std::string text, float scale);
        float lineHeight(float scale);

    private:
        gps::Shader shader;
        GLuint vao = 0;
        GLuint vbo = 0;
        std::vector<char> buffer = std::vector<char>(64000);

        void drawTriangles(std::vector<glm::vec2>& vertices, glm::vec4 color);
    };
}

#endif
