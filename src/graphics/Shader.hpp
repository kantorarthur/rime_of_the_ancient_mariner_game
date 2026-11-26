#ifndef Shader_hpp
#define Shader_hpp

#include "core/OpenGL.hpp"

#include <string>

namespace gps {

    class Shader
    {
    public:
        GLuint shaderProgram = 0;

        void loadShader(std::string vertexShaderFileName, std::string fragmentShaderFileName);
        void useShaderProgram();

    private:
        std::string readShaderFile(std::string fileName);
        void shaderCompileLog(GLuint shaderId, std::string fileName);
        void shaderLinkLog(GLuint shaderProgramId);
    };
}

#endif
