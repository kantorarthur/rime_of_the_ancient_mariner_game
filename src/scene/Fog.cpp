#include "scene/Fog.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace gps {

    const FogSettings CALM_FOG = {glm::vec3(0.024f, 0.032f, 0.046f), 0.010f, 0.045f, 3.0f};
    const FogSettings STORM_FOG = {glm::vec3(0.0045f, 0.0055f, 0.0075f), 0.018f, 0.035f, 3.0f};
    const float FOG_FADE_SPEED = 0.6f;

    void Fog::update(float deltaTime, float stormFactor)
    {
        float target = enabled ? 1.0f : 0.0f;
        float step = FOG_FADE_SPEED * deltaTime;
        amount += glm::clamp(target - amount, -step, step);

        current.color = glm::mix(CALM_FOG.color, STORM_FOG.color, stormFactor);
        current.density = glm::mix(CALM_FOG.density, STORM_FOG.density, stormFactor) * amount;
        current.heightFalloff = glm::mix(CALM_FOG.heightFalloff, STORM_FOG.heightFalloff, stormFactor);
        current.glowStrength = glm::mix(CALM_FOG.glowStrength, STORM_FOG.glowStrength, stormFactor);
    }

    void Fog::toggle()
    {
        enabled = !enabled;
    }

    bool Fog::isEnabled()
    {
        return enabled;
    }

    void Fog::upload(gps::Shader shader)
    {
        GLuint program = shader.shaderProgram;
        glUniform3fv(glGetUniformLocation(program, "fog.color"), 1, glm::value_ptr(current.color));
        glUniform1f(glGetUniformLocation(program, "fog.density"), current.density);
        glUniform1f(glGetUniformLocation(program, "fog.heightFalloff"), current.heightFalloff);
        glUniform1f(glGetUniformLocation(program, "fog.glowStrength"), current.glowStrength);
    }
}
