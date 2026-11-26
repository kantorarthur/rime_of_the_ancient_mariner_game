#include "graphics/Model3D.hpp"

#include "stb_image.h"
#include "tiny_obj_loader.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>

namespace gps {

    void Model3D::LoadModel(std::string fileName)
    {
        std::string basePath = fileName.substr(0, fileName.find_last_of('/')) + "/";
        ReadOBJ(fileName, basePath);
    }

    void Model3D::LoadModel(std::string fileName, std::string basePath)
    {
        ReadOBJ(fileName, basePath);
    }

    void Model3D::Draw(gps::Shader shaderProgram)
    {
        for(size_t i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shaderProgram);
    }

    void Model3D::ReadOBJ(std::string fileName, std::string basePath)
    {
        std::cout << "Loading : " << fileName << std::endl;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;

        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileName.c_str(), basePath.c_str(), GL_TRUE);

        if(!err.empty())
            std::cerr << err << std::endl;

        if(!ret)
            exit(1);

        for(size_t s = 0; s < shapes.size(); s++)
        {
            const tinyobj::mesh_t& shapeMesh = shapes[s].mesh;

            std::vector<size_t> faceOffsets;
            size_t offset = 0;
            for(size_t f = 0; f < shapeMesh.num_face_vertices.size(); f++)
            {
                faceOffsets.push_back(offset);
                offset += shapeMesh.num_face_vertices[f];
            }

            std::map<int, std::vector<size_t>> facesByMaterial;
            for(size_t f = 0; f < shapeMesh.num_face_vertices.size(); f++)
            {
                int materialId = -1;
                if(f < shapeMesh.material_ids.size())
                    materialId = shapeMesh.material_ids[f];
                if(materialId >= (int)materials.size())
                    materialId = -1;
                facesByMaterial[materialId].push_back(f);
            }

            for(const auto& group : facesByMaterial)
            {
                int materialId = group.first;

                std::vector<gps::Vertex> vertices;
                std::vector<GLuint> indices;

                for(size_t face : group.second)
                {
                    size_t faceOffset = faceOffsets[face];
                    int faceVertices = shapeMesh.num_face_vertices[face];

                    glm::vec3 corners[3];
                    for(int v = 0; v < 3 && v < faceVertices; v++)
                    {
                        tinyobj::index_t idx = shapeMesh.indices[faceOffset + v];
                        corners[v] = glm::vec3(
                            attrib.vertices[3 * idx.vertex_index + 0],
                            attrib.vertices[3 * idx.vertex_index + 1],
                            attrib.vertices[3 * idx.vertex_index + 2]);
                    }

                    glm::vec3 faceNormal = glm::cross(corners[1] - corners[0], corners[2] - corners[0]);
                    if(glm::length(faceNormal) > 0.0f)
                        faceNormal = glm::normalize(faceNormal);
                    else
                        faceNormal = glm::vec3(0.0f, 1.0f, 0.0f);

                    for(int v = 0; v < faceVertices; v++)
                    {
                        tinyobj::index_t idx = shapeMesh.indices[faceOffset + v];

                        gps::Vertex vertex;
                        vertex.Position = glm::vec3(
                            attrib.vertices[3 * idx.vertex_index + 0],
                            attrib.vertices[3 * idx.vertex_index + 1],
                            attrib.vertices[3 * idx.vertex_index + 2]);

                        if(idx.normal_index >= 0)
                        {
                            vertex.Normal = glm::vec3(
                                attrib.normals[3 * idx.normal_index + 0],
                                attrib.normals[3 * idx.normal_index + 1],
                                attrib.normals[3 * idx.normal_index + 2]);
                        }
                        else
                            vertex.Normal = faceNormal;

                        vertex.TexCoords = glm::vec2(0.0f);
                        if(idx.texcoord_index >= 0)
                        {
                            vertex.TexCoords = glm::vec2(
                                attrib.texcoords[2 * idx.texcoord_index + 0],
                                attrib.texcoords[2 * idx.texcoord_index + 1]);
                        }

                        indices.push_back((GLuint)vertices.size());
                        vertices.push_back(vertex);
                    }
                }

                glm::vec3 diffuseColor(1.0f);
                glm::vec3 specularColor(0.2f);
                glm::vec3 emissiveColor(0.0f);
                std::string diffuseName;
                std::string specularName;
                std::string emissiveName;

                if(materialId >= 0)
                {
                    const tinyobj::material_t& material = materials[materialId];
                    diffuseColor = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
                    specularColor = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);
                    diffuseName = material.diffuse_texname;
                    specularName = material.specular_texname;
                    emissiveColor = glm::vec3(material.emission[0], material.emission[1], material.emission[2]);
                    emissiveName = material.emissive_texname;
                }

                std::vector<gps::Texture> textures;
                textures.push_back(LoadTexture(basePath, diffuseName, "diffuseTexture", true, diffuseColor));
                textures.push_back(LoadTexture(basePath, specularName, "specularTexture", false, specularColor));
                textures.push_back(LoadTexture(basePath, emissiveName, "emissiveTexture", true, emissiveColor));

                meshes.push_back(gps::Mesh(vertices, indices, textures));
            }
        }

        std::cout << "  meshes: " << meshes.size() << ", textures: " << loadedTextures.size() << std::endl;
    }

    gps::Texture Model3D::LoadTexture(std::string basePath, std::string textureName, std::string type, bool srgb, glm::vec3 fallbackColor)
    {
        if(textureName.empty())
            return CreateSolidTexture(fallbackColor, type);

        std::string path = ResolveTexturePath(basePath, textureName);

        for(size_t i = 0; i < loadedTextures.size(); i++)
        {
            if(loadedTextures[i].path == path && loadedTextures[i].type == type)
                return loadedTextures[i];
        }

        GLuint id = ReadTextureFromFile(path, srgb);
        if(id == 0)
            return CreateSolidTexture(fallbackColor, type);

        gps::Texture texture;
        texture.id = id;
        texture.type = type;
        texture.path = path;
        loadedTextures.push_back(texture);

        return texture;
    }

    gps::Texture Model3D::CreateSolidTexture(glm::vec3 color, std::string type)
    {
        glm::vec3 clamped = glm::clamp(color, glm::vec3(0.0f), glm::vec3(1.0f));
        unsigned char pixel[4] = {
            (unsigned char)(clamped.r * 255.0f + 0.5f),
            (unsigned char)(clamped.g * 255.0f + 0.5f),
            (unsigned char)(clamped.b * 255.0f + 0.5f),
            255};

        std::string key = "solid:" + std::to_string(pixel[0]) + "," + std::to_string(pixel[1]) + "," + std::to_string(pixel[2]);

        for(size_t i = 0; i < loadedTextures.size(); i++)
        {
            if(loadedTextures[i].path == key && loadedTextures[i].type == type)
                return loadedTextures[i];
        }

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        gps::Texture texture;
        texture.id = textureID;
        texture.type = type;
        texture.path = key;
        loadedTextures.push_back(texture);

        return texture;
    }

    std::string Model3D::ResolveTexturePath(std::string basePath, std::string textureName)
    {
        for(char& c : textureName)
        {
            if(c == '\\')
                c = '/';
        }

        std::string fullPath = basePath + textureName;
        if(std::ifstream(fullPath).good())
            return fullPath;

        std::string fileOnly = textureName.substr(textureName.find_last_of('/') + 1);
        std::string flatPath = basePath + fileOnly;
        if(std::ifstream(flatPath).good())
            return flatPath;

        return fullPath;
    }

    GLuint Model3D::ReadTextureFromFile(std::string fileName, bool srgb)
    {
        int x;
        int y;
        int n;

        stbi_set_flip_vertically_on_load(1);
        unsigned char* imageData = stbi_load(fileName.c_str(), &x, &y, &n, 4);
        if(!imageData)
        {
            std::cerr << "ERROR: could not load texture " << fileName << std::endl;
            return 0;
        }

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        if(srgb)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(imageData);

        return textureID;
    }

    Model3D::~Model3D()
    {
        for(size_t i = 0; i < loadedTextures.size(); i++)
            glDeleteTextures(1, &loadedTextures[i].id);

        for(size_t i = 0; i < meshes.size(); i++)
        {
            Buffers buffers = meshes[i].getBuffers();
            glDeleteBuffers(1, &buffers.VBO);
            glDeleteBuffers(1, &buffers.EBO);
            glDeleteVertexArrays(1, &buffers.VAO);
        }
    }
}
