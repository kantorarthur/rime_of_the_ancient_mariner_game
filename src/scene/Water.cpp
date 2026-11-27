#include "scene/Water.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <string>

namespace gps {

    const float GRAVITY = 9.81f;
    const float PI = 3.14159265f;
    const float SHELTER_START = 20.0f;
    const float SHELTER_END = 55.0f;
    const float SHELTER_MINIMUM = 0.3f;

    static const Wave CALM_WAVES[WAVE_COUNT] = {
        {glm::vec2(0.80f, 0.60f), 21.0f, 0.10f, 0.35f},
        {glm::vec2(0.55f, 0.83f), 13.0f, 0.06f, 0.35f},
        {glm::vec2(0.95f, 0.30f), 8.0f, 0.04f, 0.35f},
        {glm::vec2(0.30f, 0.95f), 5.0f, 0.025f, 0.30f},
        {glm::vec2(0.70f, -0.71f), 3.2f, 0.014f, 0.30f},
        {glm::vec2(-0.20f, 0.98f), 2.1f, 0.008f, 0.30f}
    };

    static const Wave STORM_WAVES[WAVE_COUNT] = {
        {glm::vec2(0.80f, 0.60f), 36.0f, 0.95f, 0.70f},
        {glm::vec2(0.62f, 0.78f), 23.0f, 0.55f, 0.70f},
        {glm::vec2(0.93f, 0.37f), 14.5f, 0.34f, 0.70f},
        {glm::vec2(0.45f, 0.89f), 9.0f, 0.20f, 0.65f},
        {glm::vec2(0.99f, 0.10f), 5.6f, 0.11f, 0.60f},
        {glm::vec2(0.25f, 0.97f), 3.6f, 0.06f, 0.55f}
    };

    void Water::init(int rings, int segments, float radius)
    {
        std::vector<float> vertices;
        std::vector<GLuint> indices;

        for(int ring = 0; ring <= rings; ring++)
        {
            float t = (float)ring / (float)rings;
            float r = radius * std::pow(t, 2.4f);
            for(int segment = 0; segment < segments; segment++)
            {
                float angle = 2.0f * PI * (float)segment / (float)segments;
                vertices.push_back(r * std::cos(angle));
                vertices.push_back(r * std::sin(angle));
            }
        }

        for(int ring = 0; ring < rings; ring++)
        {
            for(int segment = 0; segment < segments; segment++)
            {
                GLuint a = ring * segments + segment;
                GLuint b = ring * segments + (segment + 1) % segments;
                GLuint c = (ring + 1) * segments + segment;
                GLuint d = (ring + 1) * segments + (segment + 1) % segments;
                indices.push_back(a);
                indices.push_back(c);
                indices.push_back(b);
                indices.push_back(b);
                indices.push_back(c);
                indices.push_back(d);
            }
        }

        indexCount = (GLsizei)indices.size();

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glBindVertexArray(0);

        blendWaves();
    }

    void Water::blendWaves()
    {
        for(int i = 0; i < WAVE_COUNT; i++)
        {
            Wave calm = CALM_WAVES[i];
            Wave storm = STORM_WAVES[i];
            waves[i].direction = glm::normalize(glm::mix(calm.direction, storm.direction, stormFactor));
            waves[i].wavelength = glm::mix(calm.wavelength, storm.wavelength, stormFactor);
            waves[i].amplitude = glm::mix(calm.amplitude, storm.amplitude, stormFactor);
            waves[i].steepness = glm::mix(calm.steepness, storm.steepness, stormFactor);
        }
    }

    void Water::update(float deltaTime, float stormFactor)
    {
        time += deltaTime;
        this->stormFactor = stormFactor;
        blendWaves();
    }

    float Water::shelterFactor(float x, float z)
    {
        float t = glm::clamp((std::sqrt(x * x + z * z) - SHELTER_START) / (SHELTER_END - SHELTER_START), 0.0f, 1.0f);
        t = t * t * (3.0f - 2.0f * t);
        return glm::mix(SHELTER_MINIMUM, 1.0f, t);
    }

    float Water::heightAt(float x, float z)
    {
        float height = 0.0f;
        for(int i = 0; i < WAVE_COUNT; i++)
        {
            float k = 2.0f * PI / waves[i].wavelength;
            float speed = std::sqrt(GRAVITY / k);
            float phase = k * (glm::dot(waves[i].direction, glm::vec2(x, z)) - speed * time);
            height += waves[i].amplitude * std::sin(phase);
        }
        return height * shelterFactor(x, z);
    }

    void Water::addFoamCircle(glm::vec2 center, float radius, float width)
    {
        if((int)foamCircles.size() < MAX_FOAM_CIRCLES)
            foamCircles.push_back(glm::vec4(center.x, center.y, radius, width));
    }

    void Water::draw(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition, GLuint skyboxTexture)
    {
        shader.useShaderProgram();
        GLuint program = shader.shaderProgram;

        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(glGetUniformLocation(program, "cameraPosition"), 1, glm::value_ptr(cameraPosition));
        glUniform2f(glGetUniformLocation(program, "gridCenter"), std::floor(cameraPosition.x), std::floor(cameraPosition.z));
        glUniform1f(glGetUniformLocation(program, "time"), time);
        glUniform1f(glGetUniformLocation(program, "stormFactor"), stormFactor);
        glUniform1f(glGetUniformLocation(program, "curse"), curse);
        glUniform3f(glGetUniformLocation(program, "shelter"), SHELTER_START, SHELTER_END, SHELTER_MINIMUM);

        for(int i = 0; i < WAVE_COUNT; i++)
        {
            std::string name = "waves[" + std::to_string(i) + "]";
            glUniform2fv(glGetUniformLocation(program, (name + ".direction").c_str()), 1, glm::value_ptr(waves[i].direction));
            glUniform1f(glGetUniformLocation(program, (name + ".wavelength").c_str()), waves[i].wavelength);
            glUniform1f(glGetUniformLocation(program, (name + ".amplitude").c_str()), waves[i].amplitude);
            glUniform1f(glGetUniformLocation(program, (name + ".steepness").c_str()), waves[i].steepness);
        }

        glUniform1i(glGetUniformLocation(program, "foamCircleCount"), (GLint)foamCircles.size());
        if(!foamCircles.empty())
            glUniform4fv(glGetUniformLocation(program, "foamCircles"), (GLsizei)foamCircles.size(), glm::value_ptr(foamCircles[0]));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glUniform1i(glGetUniformLocation(program, "skybox"), 0);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Water::setCurse(float value)
    {
        curse = value;
    }

    Water::~Water()
    {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteVertexArrays(1, &vao);
    }
}
