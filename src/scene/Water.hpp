#ifndef Water_hpp
#define Water_hpp

#include "core/OpenGL.hpp"
#include "graphics/Shader.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace gps {

    const int WAVE_COUNT = 6;
    const int MAX_FOAM_CIRCLES = 12;

    struct Wave
    {
        glm::vec2 direction;
        float wavelength;
        float amplitude;
        float steepness;
    };

    class Water
    {
    public:
        Water() = default;
        ~Water();
        Water(const Water&) = delete;
        Water& operator=(const Water&) = delete;

        void init(int rings, int segments, float radius);
        void update(float deltaTime, float stormFactor);
        void draw(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition, GLuint skyboxTexture);

        float heightAt(float x, float z);
        void addFoamCircle(glm::vec2 center, float radius, float width);

    private:
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        GLsizei indexCount = 0;

        float time = 0.0f;
        float stormFactor = 0.0f;
        Wave waves[WAVE_COUNT];
        std::vector<glm::vec4> foamCircles;

        void blendWaves();
        float shelterFactor(float x, float z);
    };
}

#endif
