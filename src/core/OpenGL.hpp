#ifndef OpenGL_hpp
#define OpenGL_hpp

#if defined(__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
    #include <OpenGL/gl3.h>
#else
    #ifndef GLEW_STATIC
        #define GLEW_STATIC
    #endif
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#endif
