#include "graphics/Lights.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <cmath>

namespace gps {

    static void setVec3(GLuint program, std::string name, glm::vec3 value)
    {
        glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, glm::value_ptr(value));
    }

    static void setFloat(GLuint program, std::string name, float value)
    {
        glUniform1f(glGetUniformLocation(program, name.c_str()), value);
    }

    void uploadDirectionalLight(GLuint program, std::string name, DirectionalLight light)
    {
        setVec3(program, name + ".direction", glm::normalize(light.direction));
        setVec3(program, name + ".color", light.color);
        setVec3(program, name + ".ambient", light.ambient);
    }

    void uploadPointLights(GLuint program, std::vector<PointLight> lights)
    {
        int count = std::min((int)lights.size(), MAX_POINT_LIGHTS);
        glUniform1i(glGetUniformLocation(program, "pointLightCount"), count);

        for(int i = 0; i < count; i++)
        {
            std::string name = "pointLights[" + std::to_string(i) + "]";
            setVec3(program, name + ".position", lights[i].position);
            setVec3(program, name + ".color", lights[i].color);
            setFloat(program, name + ".constant", lights[i].constant);
            setFloat(program, name + ".linear", lights[i].linear);
            setFloat(program, name + ".quadratic", lights[i].quadratic);
        }
    }

    void uploadSpotLight(GLuint program, std::string name, SpotLight light)
    {
        setVec3(program, name + ".position", light.position);
        setVec3(program, name + ".direction", glm::normalize(light.direction));
        setVec3(program, name + ".color", light.color);
        setFloat(program, name + ".cutOff", std::cos(glm::radians(light.cutOffDegrees)));
        setFloat(program, name + ".outerCutOff", std::cos(glm::radians(light.outerCutOffDegrees)));
        setFloat(program, name + ".constant", light.constant);
        setFloat(program, name + ".linear", light.linear);
        setFloat(program, name + ".quadratic", light.quadratic);
    }
}
