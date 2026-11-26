#include "graphics/Shader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace gps {

    std::string Shader::readShaderFile(std::string fileName)
    {
        std::ifstream shaderFile(fileName);
        if(!shaderFile.is_open())
        {
            std::cerr << "Could not open shader file: " << fileName << std::endl;
            return "";
        }

        std::string folder = fileName.substr(0, fileName.find_last_of('/') + 1);
        std::stringstream shaderStringStream;
        std::string line;
        while(std::getline(shaderFile, line))
        {
            if(line.rfind("#include", 0) == 0)
            {
                size_t first = line.find('"');
                size_t last = line.find_last_of('"');
                shaderStringStream << readShaderFile(folder + line.substr(first + 1, last - first - 1));
            }
            else
                shaderStringStream << line << "\n";
        }
        return shaderStringStream.str();
    }

    void Shader::shaderCompileLog(GLuint shaderId, std::string fileName)
    {
        GLint success;
        GLchar infoLog[512];

        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
            std::cerr << "Shader compilation error in " << fileName << "\n" << infoLog << std::endl;
        }
    }

    void Shader::shaderLinkLog(GLuint shaderProgramId)
    {
        GLint success;
        GLchar infoLog[512];

        glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(shaderProgramId, 512, NULL, infoLog);
            std::cerr << "Shader linking error\n" << infoLog << std::endl;
        }
    }

    void Shader::loadShader(std::string vertexShaderFileName, std::string fragmentShaderFileName)
    {
        std::string vertexSource = readShaderFile(vertexShaderFileName);
        const GLchar* vertexShaderString = vertexSource.c_str();
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderString, NULL);
        glCompileShader(vertexShader);
        shaderCompileLog(vertexShader, vertexShaderFileName);

        std::string fragmentSource = readShaderFile(fragmentShaderFileName);
        const GLchar* fragmentShaderString = fragmentSource.c_str();
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderString, NULL);
        glCompileShader(fragmentShader);
        shaderCompileLog(fragmentShader, fragmentShaderFileName);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        shaderLinkLog(shaderProgram);
    }

    void Shader::useShaderProgram()
    {
        glUseProgram(shaderProgram);
    }
}
