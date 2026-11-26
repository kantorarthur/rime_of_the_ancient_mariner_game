#ifndef Lights_hpp
#define Lights_hpp

#include "core/OpenGL.hpp"

#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace gps {

    const int MAX_POINT_LIGHTS = 8;

    struct DirectionalLight
    {
        glm::vec3 direction;
        glm::vec3 color;
        glm::vec3 ambient;
    };

    struct PointLight
    {
        glm::vec3 position;
        glm::vec3 color;
        float constant;
        float linear;
        float quadratic;
    };

    struct SpotLight
    {
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 color;
        float cutOffDegrees;
        float outerCutOffDegrees;
        float constant;
        float linear;
        float quadratic;
    };

    void uploadDirectionalLight(GLuint program, std::string name, DirectionalLight light);
    void uploadPointLights(GLuint program, std::vector<PointLight> lights);
    void uploadSpotLight(GLuint program, std::string name, SpotLight light);
}

#endif
