#ifndef SkinnedMesh_hpp
#define SkinnedMesh_hpp

#include "core/OpenGL.hpp"
#include "graphics/Shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <vector>

namespace gps {

    const int MAX_SKIN_BONES = 32;

    struct Bone
    {
        std::string name;
        int parent;
        glm::vec3 restTranslation;
        glm::quat restRotation;
        glm::mat4 inverseBind;
    };

    struct SkinnedSubmesh
    {
        GLuint texture;
        GLuint firstIndex;
        GLuint indexCount;
    };

    class SkinnedMesh
    {
    public:
        SkinnedMesh() = default;
        ~SkinnedMesh();
        SkinnedMesh(const SkinnedMesh&) = delete;
        SkinnedMesh& operator=(const SkinnedMesh&) = delete;

        bool load(std::string fileName, std::string textureFolder);
        void draw(gps::Shader shader, std::vector<glm::mat4>& palette);

        std::vector<Bone>& getBones();
        int findBone(std::string name);

    private:
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        GLuint specularTexture = 0;
        GLuint emissiveTexture = 0;

        std::vector<Bone> bones;
        std::vector<SkinnedSubmesh> submeshes;
        std::vector<std::string> textureNames;
        std::vector<GLuint> textures;

        GLuint loadTexture(std::string path);
        GLuint solidTexture(unsigned char value);
    };
}

#endif
