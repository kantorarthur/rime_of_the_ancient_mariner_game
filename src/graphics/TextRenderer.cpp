#include "graphics/TextRenderer.hpp"

#include "stb_easy_font.h"

#include <glm/gtc/type_ptr.hpp>

namespace gps {

    struct FontVertex
    {
        float x;
        float y;
        float z;
        unsigned char color[4];
    };

    const float LINE_SPACING = 12.0f;

    void TextRenderer::init(gps::Shader shader)
    {
        this->shader = shader;

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
        glBindVertexArray(0);
    }

    void TextRenderer::begin(int screenWidth, int screenHeight)
    {
        shader.useShaderProgram();
        glUniform2f(glGetUniformLocation(shader.shaderProgram, "screenSize"), (float)screenWidth, (float)screenHeight);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
    }

    void TextRenderer::end()
    {
        glBindVertexArray(0);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

    void TextRenderer::drawTriangles(std::vector<glm::vec2>& vertices, glm::vec4 color)
    {
        if(vertices.empty())
            return;

        glUniform4fv(glGetUniformLocation(shader.shaderProgram, "color"), 1, glm::value_ptr(color));
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_STREAM_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
    }

    void TextRenderer::drawText(std::string text, float x, float y, float scale, glm::vec4 color)
    {
        int quadCount = stb_easy_font_print(0.0f, 0.0f, (char*)text.c_str(), nullptr, buffer.data(), (int)buffer.size());
        FontVertex* quads = (FontVertex*)buffer.data();

        std::vector<glm::vec2> vertices;
        for(int i = 0; i < quadCount; i++)
        {
            glm::vec2 corners[4];
            for(int j = 0; j < 4; j++)
                corners[j] = glm::vec2(x + quads[4 * i + j].x * scale, y + quads[4 * i + j].y * scale);
            vertices.insert(vertices.end(), {corners[0], corners[1], corners[2], corners[0], corners[2], corners[3]});
        }

        std::vector<glm::vec2> shadow = vertices;
        for(glm::vec2& vertex : shadow)
            vertex += glm::vec2(scale * 0.6f);

        drawTriangles(shadow, glm::vec4(0.0f, 0.0f, 0.0f, color.a * 0.8f));
        drawTriangles(vertices, color);
    }

    void TextRenderer::drawTextCentered(std::string text, float centerX, float y, float scale, glm::vec4 color)
    {
        drawText(text, centerX - measure(text, scale) * 0.5f, y, scale, color);
    }

    void TextRenderer::drawRectangle(float x, float y, float width, float height, glm::vec4 color)
    {
        std::vector<glm::vec2> vertices = {
            glm::vec2(x, y), glm::vec2(x + width, y), glm::vec2(x + width, y + height),
            glm::vec2(x, y), glm::vec2(x + width, y + height), glm::vec2(x, y + height)
        };
        drawTriangles(vertices, color);
    }

    float TextRenderer::measure(std::string text, float scale)
    {
        return stb_easy_font_width((char*)text.c_str()) * scale;
    }

    float TextRenderer::lineHeight(float scale)
    {
        return LINE_SPACING * scale;
    }

    TextRenderer::~TextRenderer()
    {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }
}
