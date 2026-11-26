#include "graphics/SkinnedMesh.hpp"
#include "graphics/Mesh.hpp"

#include "stb_image.h"

#include <glm/gtc/type_ptr.hpp>

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>

namespace gps {

    struct SkinnedVertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
        uint8_t boneIds[4];
        float weights[4];
    };

    template<typename T>
    static T readValue(std::ifstream& file)
    {
        T value;
        file.read((char*)&value, sizeof(T));
        return value;
    }

    static std::string readName(std::ifstream& file, int length)
    {
        std::vector<char> text(length + 1, 0);
        file.read(text.data(), length);
        return std::string(text.data());
    }

    bool SkinnedMesh::load(std::string fileName, std::string textureFolder)
    {
        std::ifstream file(fileName, std::ios::binary);
        char magic[4];
        file.read(magic, 4);
        if(!file || std::strncmp(magic, "SKIN", 4) != 0)
        {
            std::cerr << "ERROR: could not load skinned mesh " << fileName << std::endl;
            return false;
        }
        readValue<uint32_t>(file);

        uint32_t boneCount = readValue<uint32_t>(file);
        for(uint32_t i = 0; i < boneCount; i++)
        {
            Bone bone;
            bone.name = readName(file, 32);
            bone.parent = readValue<int32_t>(file);
            file.read((char*)glm::value_ptr(bone.restTranslation), 3 * sizeof(float));
            float rotation[4];
            file.read((char*)rotation, 4 * sizeof(float));
            bone.restRotation = glm::quat(rotation[3], rotation[0], rotation[1], rotation[2]);
            file.read((char*)glm::value_ptr(bone.inverseBind), 16 * sizeof(float));
            bones.push_back(bone);
        }

        uint32_t vertexCount = readValue<uint32_t>(file);
        std::vector<SkinnedVertex> vertices(vertexCount);
        for(SkinnedVertex& vertex : vertices)
        {
            file.read((char*)glm::value_ptr(vertex.position), 3 * sizeof(float));
            file.read((char*)glm::value_ptr(vertex.normal), 3 * sizeof(float));
            file.read((char*)glm::value_ptr(vertex.texCoords), 2 * sizeof(float));
            file.read((char*)vertex.boneIds, 4);
            file.read((char*)vertex.weights, 4 * sizeof(float));
        }

        uint32_t submeshCount = readValue<uint32_t>(file);
        for(uint32_t i = 0; i < submeshCount; i++)
        {
            std::string textureName = readName(file, 64);
            SkinnedSubmesh submesh;
            submesh.firstIndex = readValue<uint32_t>(file);
            submesh.indexCount = readValue<uint32_t>(file);

            submesh.texture = 0;
            for(size_t j = 0; j < textureNames.size(); j++)
            {
                if(textureNames[j] == textureName)
                    submesh.texture = textures[j];
            }
            if(submesh.texture == 0)
            {
                submesh.texture = loadTexture(textureFolder + "/" + textureName);
                textureNames.push_back(textureName);
                textures.push_back(submesh.texture);
            }
            submeshes.push_back(submesh);
        }

        uint32_t indexCount = readValue<uint32_t>(file);
        std::vector<uint32_t> indices(indexCount);
        file.read((char*)indices.data(), indexCount * sizeof(uint32_t));

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SkinnedVertex), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, texCoords));
        glEnableVertexAttribArray(3);
        glVertexAttribIPointer(3, 4, GL_UNSIGNED_BYTE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, boneIds));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, weights));
        glBindVertexArray(0);

        specularTexture = solidTexture(20);
        emissiveTexture = solidTexture(0);
        return true;
    }

    GLuint SkinnedMesh::loadTexture(std::string path)
    {
        int width;
        int height;
        int channels;
        stbi_set_flip_vertically_on_load(1);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
        if(!data)
        {
            std::cerr << "ERROR: could not load texture " << path << std::endl;
            return solidTexture(128);
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        stbi_image_free(data);
        return texture;
    }

    GLuint SkinnedMesh::solidTexture(unsigned char value)
    {
        unsigned char pixel[4] = {value, value, value, 255};
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        return texture;
    }

    void SkinnedMesh::draw(gps::Shader shader, std::vector<glm::mat4>& palette)
    {
        GLuint program = shader.shaderProgram;
        glUniformMatrix4fv(glGetUniformLocation(program, "bones"), (GLsizei)palette.size(), GL_FALSE, glm::value_ptr(palette[0]));

        glActiveTexture(GL_TEXTURE0 + SPECULAR_TEXTURE_UNIT);
        glBindTexture(GL_TEXTURE_2D, specularTexture);
        glUniform1i(glGetUniformLocation(program, "specularTexture"), SPECULAR_TEXTURE_UNIT);
        glActiveTexture(GL_TEXTURE0 + EMISSIVE_TEXTURE_UNIT);
        glBindTexture(GL_TEXTURE_2D, emissiveTexture);
        glUniform1i(glGetUniformLocation(program, "emissiveTexture"), EMISSIVE_TEXTURE_UNIT);
        glUniform1i(glGetUniformLocation(program, "diffuseTexture"), DIFFUSE_TEXTURE_UNIT);

        glBindVertexArray(vao);
        for(SkinnedSubmesh& submesh : submeshes)
        {
            glActiveTexture(GL_TEXTURE0 + DIFFUSE_TEXTURE_UNIT);
            glBindTexture(GL_TEXTURE_2D, submesh.texture);
            glDrawElements(GL_TRIANGLES, submesh.indexCount, GL_UNSIGNED_INT, (void*)(submesh.firstIndex * sizeof(uint32_t)));
        }
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }

    std::vector<Bone>& SkinnedMesh::getBones()
    {
        return bones;
    }

    int SkinnedMesh::findBone(std::string name)
    {
        for(size_t i = 0; i < bones.size(); i++)
        {
            if(bones[i].name == name)
                return (int)i;
        }
        return -1;
    }

    SkinnedMesh::~SkinnedMesh()
    {
        for(GLuint texture : textures)
            glDeleteTextures(1, &texture);
        glDeleteTextures(1, &specularTexture);
        glDeleteTextures(1, &emissiveTexture);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteVertexArrays(1, &vao);
    }
}
