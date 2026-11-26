#ifndef Mesh_hpp
#define Mesh_hpp

#include "core/OpenGL.hpp"
#include "graphics/Shader.hpp"

#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace gps {

    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    struct Texture
    {
        GLuint id;
        std::string type;
        std::string path;
    };

    struct Material
    {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };

    struct Buffers
    {
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
    };

    const GLuint DIFFUSE_TEXTURE_UNIT = 0;
    const GLuint SPECULAR_TEXTURE_UNIT = 1;
    const GLuint EMISSIVE_TEXTURE_UNIT = 2;
    const GLuint AMBIENT_TEXTURE_UNIT = 3;

    class Mesh
    {
    public:
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;

        Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);

        Buffers getBuffers();
        void Draw(gps::Shader shader);

    private:
        Buffers buffers;

        void setupMesh();
        static GLuint textureUnitFor(std::string type);
    };
}

#endif
