#include "core/Window.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace gps {

    void Window::Create(int width, int height, const char* title)
    {
        if(!glfwInit())
            throw std::runtime_error("Could not start GLFW3!");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
        glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        window = glfwCreateWindow(width, height, title, NULL, NULL);
        if(!window)
            throw std::runtime_error("Could not create GLFW3 window!");

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

#if !defined(__APPLE__)
        glewExperimental = GL_TRUE;
        GLenum glewStatus = glewInit();
        if(glewStatus != GLEW_OK && glewStatus != GLEW_ERROR_NO_GLX_DISPLAY)
            throw std::runtime_error(std::string("Could not start GLEW: ") + (const char*)glewGetErrorString(glewStatus));
        glGetError();
#endif

        std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
        std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

        glfwGetFramebufferSize(window, &dimensions.width, &dimensions.height);
    }

    void Window::Delete()
    {
        if(window)
            glfwDestroyWindow(window);
        glfwTerminate();
    }

    GLFWwindow* Window::getWindow()
    {
        return window;
    }

    WindowDimensions Window::getWindowDimensions()
    {
        return dimensions;
    }

    void Window::setWindowDimensions(WindowDimensions dimensions)
    {
        this->dimensions = dimensions;
    }
}
