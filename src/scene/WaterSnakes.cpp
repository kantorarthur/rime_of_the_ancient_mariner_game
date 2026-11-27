#include "scene/WaterSnakes.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <cstddef>
#include <random>

namespace gps {

    struct SnakeVertex
    {
        glm::vec3 position;
        glm::vec4 color;
    };

    const int TRAIL_LENGTH = 90;
    const float SAMPLE_INTERVAL = 0.03f;
    const float SNAKE_WIDTH = 0.28f;
    const float FADE_SPEED = 0.35f;
    const glm::vec3 SNAKE_COLORS[] = {glm::vec3(0.15f, 0.45f, 1.0f), glm::vec3(0.1f, 0.85f, 0.45f), glm::vec3(0.25f, 0.25f, 0.55f)};
    const glm::vec3 GOLDEN_FIRE = glm::vec3(1.0f, 0.75f, 0.25f);

    void WaterSnakes::init(int count)
    {
        std::mt19937 random(11);
        std::uniform_real_distribution<float> unit(0.0f, 1.0f);
        for(int i = 0; i < count; i++)
        {
            WaterSnake snake;
            snake.angle = unit(random) * glm::two_pi<float>();
            snake.radius = 19.0f + unit(random) * 22.0f;
            snake.speed = (0.25f + unit(random) * 0.3f) * (i % 2 == 0 ? 1.0f : -1.0f);
            snake.phase = unit(random) * 10.0f;
            snake.color = SNAKE_COLORS[i % 3];
            snakes.push_back(snake);
        }

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SnakeVertex), (void*)offsetof(SnakeVertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SnakeVertex), (void*)offsetof(SnakeVertex, color));
        glBindVertexArray(0);
    }

    void WaterSnakes::show(bool visible)
    {
        targetIntensity = visible ? 1.0f : 0.0f;
    }

    void WaterSnakes::update(float deltaTime, glm::vec3 center, gps::Water& water)
    {
        float step = FADE_SPEED * deltaTime;
        intensity += glm::clamp(targetIntensity - intensity, -step, step);
        if(intensity <= 0.0f && targetIntensity <= 0.0f)
            return;

        time += deltaTime;
        sampleTimer += deltaTime;
        bool sample = sampleTimer >= SAMPLE_INTERVAL;
        if(sample)
            sampleTimer = 0.0f;

        for(WaterSnake& snake : snakes)
        {
            snake.angle += snake.speed / snake.radius * 4.0f * deltaTime;
            float wander = snake.radius + 3.0f * std::sin(time * 0.3f + snake.phase);
            float sway = 0.9f * std::sin(time * 1.6f + snake.phase);
            glm::vec2 around = glm::vec2(std::cos(snake.angle), std::sin(snake.angle));
            glm::vec2 side = glm::vec2(-around.y, around.x);
            glm::vec2 point = glm::vec2(center.x, center.z) + around * wander + side * sway;

            float rearing = glm::max(0.0f, std::sin(time * 0.7f + snake.phase * 3.0f) - 0.85f) * 6.0f;
            glm::vec3 head = glm::vec3(point.x, water.heightAt(point.x, point.y) + 0.05f + rearing, point.y);

            if(snake.trail.empty())
                snake.trail.assign(TRAIL_LENGTH, head);
            if(sample)
            {
                snake.trail.insert(snake.trail.begin(), head);
                snake.trail.pop_back();
            }
            snake.trail[0] = head;
            for(size_t i = 1; i < snake.trail.size(); i++)
            {
                glm::vec3& body = snake.trail[i];
                body.y = glm::mix(body.y, water.heightAt(body.x, body.z) + 0.05f, 0.1f);
            }
        }
    }

    void WaterSnakes::draw(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition)
    {
        if(intensity <= 0.0f)
            return;

        std::vector<SnakeVertex> vertices;
        for(WaterSnake& snake : snakes)
        {
            std::vector<glm::vec3> left;
            std::vector<glm::vec3> right;
            std::vector<glm::vec4> colors;
            for(size_t i = 0; i < snake.trail.size(); i++)
            {
                size_t next = glm::min(i + 1, snake.trail.size() - 1);
                size_t previous = i > 0 ? i - 1 : 0;
                glm::vec3 along = snake.trail[next] - snake.trail[previous];
                glm::vec3 side = glm::cross(along, snake.trail[i] - cameraPosition);
                if(glm::length(side) < 0.0001f)
                    side = glm::vec3(1.0f, 0.0f, 0.0f);
                side = glm::normalize(side);

                float t = (float)i / (TRAIL_LENGTH - 1);
                float width = SNAKE_WIDTH * glm::mix(0.75f, 1.0f, glm::smoothstep(0.0f, 0.08f, t)) * (1.0f - t * t);
                float flash = glm::max(0.0f, std::sin(t * 30.0f - time * 6.0f + snake.phase));
                flash = flash * flash * flash * flash;
                glm::vec3 color = glm::mix(snake.color, GOLDEN_FIRE, flash);
                color = glm::mix(glm::vec3(1.0f), color, glm::smoothstep(0.0f, 0.1f, t));

                left.push_back(snake.trail[i] - side * width);
                right.push_back(snake.trail[i] + side * width);
                colors.push_back(glm::vec4(color, intensity * (1.0f - t)));
            }
            for(size_t i = 0; i + 1 < snake.trail.size(); i++)
            {
                glm::vec3 middle0 = (left[i] + right[i]) * 0.5f;
                glm::vec3 middle1 = (left[i + 1] + right[i + 1]) * 0.5f;
                glm::vec4 edge0 = glm::vec4(glm::vec3(colors[i]), 0.0f);
                glm::vec4 edge1 = glm::vec4(glm::vec3(colors[i + 1]), 0.0f);

                vertices.push_back({left[i], edge0});
                vertices.push_back({middle0, colors[i]});
                vertices.push_back({left[i + 1], edge1});
                vertices.push_back({middle0, colors[i]});
                vertices.push_back({middle1, colors[i + 1]});
                vertices.push_back({left[i + 1], edge1});

                vertices.push_back({middle0, colors[i]});
                vertices.push_back({right[i], edge0});
                vertices.push_back({middle1, colors[i + 1]});
                vertices.push_back({right[i], edge0});
                vertices.push_back({right[i + 1], edge1});
                vertices.push_back({middle1, colors[i + 1]});
            }
        }
        if(vertices.empty())
            return;

        shader.useShaderProgram();
        GLuint program = shader.shaderProgram;
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SnakeVertex), vertices.data(), GL_STREAM_DRAW);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glBindVertexArray(0);
    }

    glm::vec3 WaterSnakes::getHeadPosition(int snake)
    {
        if(snakes[snake].trail.empty())
            return glm::vec3(0.0f);
        return snakes[snake].trail[0];
    }

    WaterSnakes::~WaterSnakes()
    {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }
}
