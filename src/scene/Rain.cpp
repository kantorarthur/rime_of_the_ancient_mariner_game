#include "scene/Rain.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <vector>

namespace gps {

    const glm::vec3 RAIN_BOX_SIZE = glm::vec3(50.0f, 30.0f, 50.0f);
    const float RAIN_FALL_SPEED = 10.0f;
    const float RAIN_STREAK_TIME = 0.035f;
    const float RAIN_STREAK_WIDTH = 0.018f;
    const float RAIN_OPACITY = 0.45f;

    void Rain::init(int count)
    {
        this->count = count;

        std::mt19937 random(7);
        std::uniform_real_distribution<float> unit(0.0f, 1.0f);
        std::vector<glm::vec3> offsets;
        for(int i = 0; i < count; i++)
            offsets.push_back(glm::vec3(unit(random), unit(random), unit(random)));

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(glm::vec3), offsets.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glVertexAttribDivisor(0, 1);
        glBindVertexArray(0);
    }

    void Rain::update(float deltaTime, glm::vec3 windVelocity, float intensity)
    {
        this->intensity = intensity;
        velocity = windVelocity + glm::vec3(0.0f, -RAIN_FALL_SPEED, 0.0f);
        displacement = glm::mod(displacement + velocity * deltaTime, RAIN_BOX_SIZE);
    }

    void Rain::draw(gps::Shader shader, glm::mat4 view, glm::mat4 projection)
    {
        int visibleDrops = (int)(count * intensity);
        if(visibleDrops <= 0)
            return;

        shader.useShaderProgram();
        GLuint program = shader.shaderProgram;
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(glGetUniformLocation(program, "boxSize"), 1, glm::value_ptr(RAIN_BOX_SIZE));
        glUniform3fv(glGetUniformLocation(program, "displacement"), 1, glm::value_ptr(displacement));
        glUniform3fv(glGetUniformLocation(program, "velocity"), 1, glm::value_ptr(velocity));
        glUniform1f(glGetUniformLocation(program, "streakLength"), glm::length(velocity) * RAIN_STREAK_TIME);
        glUniform1f(glGetUniformLocation(program, "streakWidth"), RAIN_STREAK_WIDTH);
        glUniform1f(glGetUniformLocation(program, "rainOpacity"), RAIN_OPACITY);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);

        glBindVertexArray(vao);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, visibleDrops);
        glBindVertexArray(0);

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    Rain::~Rain()
    {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }
}
