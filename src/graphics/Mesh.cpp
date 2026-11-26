#include "graphics/Mesh.hpp"

#include <cstddef>

namespace gps {

    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        setupMesh();
    }

    Buffers Mesh::getBuffers()
    {
        return buffers;
    }

    GLuint Mesh::textureUnitFor(std::string type)
    {
        if(type == "diffuseTexture")
            return DIFFUSE_TEXTURE_UNIT;
        if(type == "specularTexture")
            return SPECULAR_TEXTURE_UNIT;
        if(type == "emissiveTexture")
            return EMISSIVE_TEXTURE_UNIT;
        return AMBIENT_TEXTURE_UNIT;
    }

    void Mesh::Draw(gps::Shader shader)
    {
        shader.useShaderProgram();

        for(size_t i = 0; i < textures.size(); i++)
        {
            GLuint unit = textureUnitFor(textures[i].type);
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
            glUniform1i(glGetUniformLocation(shader.shaderProgram, textures[i].type.c_str()), (GLint)unit);
        }

        glBindVertexArray(buffers.VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }

    void Mesh::setupMesh()
    {
        glGenVertexArrays(1, &buffers.VAO);
        glGenBuffers(1, &buffers.VBO);
        glGenBuffers(1, &buffers.EBO);

        glBindVertexArray(buffers.VAO);

        glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }
}
