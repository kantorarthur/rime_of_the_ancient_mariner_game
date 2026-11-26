#include "scene/LightBeam.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <vector>

namespace gps {

    const float BEAM_STRENGTH = 12.0f;
    const float CLEAR_AIR_DENSITY = 0.0015f;

    void LightBeam::init(float length, float startRadius, float spreadDegrees, int segments)
    {
        this->length = length;
        this->startRadius = startRadius;
        spread = std::tan(glm::radians(spreadDegrees));
        float endRadius = startRadius + length * spread;

        std::vector<glm::vec3> vertices;
        for(int i = 0; i <= segments; i++)
        {
            float angle = 2.0f * glm::pi<float>() * i / segments;
            glm::vec2 around = glm::vec2(std::cos(angle), std::sin(angle));
            vertices.push_back(glm::vec3(around * startRadius, 0.0f));
            vertices.push_back(glm::vec3(around * endRadius, length));
        }

        GLuint startCenter = (GLuint)vertices.size();
        vertices.push_back(glm::vec3(0.0f));
        GLuint endCenter = (GLuint)vertices.size();
        vertices.push_back(glm::vec3(0.0f, 0.0f, length));

        std::vector<GLuint> indices;
        for(int i = 0; i < segments; i++)
        {
            GLuint start = 2 * i;
            indices.insert(indices.end(), {start, start + 2, start + 1, start + 1, start + 2, start + 3});
            indices.insert(indices.end(), {startCenter, start + 2, start});
            indices.insert(indices.end(), {endCenter, start + 1, start + 3});
        }
        indexCount = (GLsizei)indices.size();

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glBindVertexArray(0);
    }

    void LightBeam::draw(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 apex, glm::vec3 direction)
    {
        glm::vec3 forward = glm::normalize(direction);
        glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), forward));
        glm::vec3 up = glm::cross(forward, right);
        glm::mat4 model = glm::mat4(glm::vec4(right, 0.0f), glm::vec4(up, 0.0f), glm::vec4(forward, 0.0f), glm::vec4(apex, 1.0f));

        shader.useShaderProgram();
        GLuint program = shader.shaderProgram;
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1f(glGetUniformLocation(program, "shaftLength"), length);
        glUniform1f(glGetUniformLocation(program, "shaftStartRadius"), startRadius);
        glUniform1f(glGetUniformLocation(program, "shaftSpread"), spread);
        glUniform1f(glGetUniformLocation(program, "shaftStrength"), BEAM_STRENGTH);
        glUniform1f(glGetUniformLocation(program, "clearAirDensity"), CLEAR_AIR_DENSITY);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glDepthMask(GL_FALSE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glDisable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    LightBeam::~LightBeam()
    {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteVertexArrays(1, &vao);
    }
}
