#ifndef Model3D_hpp
#define Model3D_hpp

#include "graphics/Mesh.hpp"

#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace gps {

    class Model3D
    {
    public:
        Model3D() = default;
        ~Model3D();

        Model3D(const Model3D&) = delete;
        Model3D& operator=(const Model3D&) = delete;

        void LoadModel(std::string fileName);
        void LoadModel(std::string fileName, std::string basePath);
        void Draw(gps::Shader shaderProgram);

    private:
        std::vector<gps::Mesh> meshes;
        std::vector<gps::Texture> loadedTextures;

        void ReadOBJ(std::string fileName, std::string basePath);
        gps::Texture LoadTexture(std::string basePath, std::string textureName, std::string type, bool srgb, glm::vec3 fallbackColor);
        gps::Texture CreateSolidTexture(glm::vec3 color, std::string type);
        GLuint ReadTextureFromFile(std::string fileName, bool srgb);
        std::string ResolveTexturePath(std::string basePath, std::string textureName);
    };
}

#endif
