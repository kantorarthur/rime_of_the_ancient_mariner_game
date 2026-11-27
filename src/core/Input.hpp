#ifndef Input_hpp
#define Input_hpp

#include "core/OpenGL.hpp"

#include <glm/glm.hpp>

namespace gps {

    class Input
    {
    public:
        void attach(GLFWwindow* window);
        void endFrame();

        bool isKeyDown(int key);
        bool wasKeyPressed(int key);

        glm::vec2 getMouseDelta();
        float getScrollDelta();
        void setCursorCaptured(bool captured);
        bool isCursorCaptured();

        bool wasResized();
        int getFramebufferWidth();
        int getFramebufferHeight();

    private:
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void cursorCallback(GLFWwindow* window, double xpos, double ypos);
        static void framebufferCallback(GLFWwindow* window, int width, int height);
        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

        static constexpr int KEY_COUNT = GLFW_KEY_LAST + 1;

        GLFWwindow* window = nullptr;

        bool keysDown[KEY_COUNT] = {};
        bool keysPressed[KEY_COUNT] = {};

        glm::vec2 mouseDelta = glm::vec2(0.0f);
        float scrollDelta = 0.0f;
        double lastMouseX = 0.0;
        double lastMouseY = 0.0;
        bool firstMouse = true;
        bool cursorCaptured = false;

        bool resized = false;
        int framebufferWidth = 0;
        int framebufferHeight = 0;
    };
}

#endif
