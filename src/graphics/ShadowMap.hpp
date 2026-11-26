#ifndef ShadowMap_hpp
#define ShadowMap_hpp

#include "core/OpenGL.hpp"

namespace gps {

    class ShadowMap
    {
    public:
        ShadowMap() = default;
        ~ShadowMap();
        ShadowMap(const ShadowMap&) = delete;
        ShadowMap& operator=(const ShadowMap&) = delete;

        void init(int size);
        void begin();
        void end();
        void bind(GLuint program, const char* samplerName, GLuint unit);
        int getSize();

    private:
        GLuint framebuffer = 0;
        GLuint depthTexture = 0;
        int size = 0;
    };
}

#endif
