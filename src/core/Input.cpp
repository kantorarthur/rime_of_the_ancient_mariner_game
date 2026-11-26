#include "core/Input.hpp"

namespace gps {

    void Input::attach(GLFWwindow* window)
    {
        this->window = window;
        glfwSetWindowUserPointer(window, this);

        glfwSetKeyCallback(window, keyCallback);
        glfwSetCursorPosCallback(window, cursorCallback);
        glfwSetFramebufferSizeCallback(window, framebufferCallback);

        if(glfwRawMouseMotionSupported())
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        setCursorCaptured(true);
    }

    void Input::endFrame()
    {
        for(int i = 0; i < KEY_COUNT; i++)
            keysPressed[i] = false;

        mouseDelta = glm::vec2(0.0f);
        resized = false;
    }

    bool Input::isKeyDown(int key)
    {
        if(key < 0 || key >= KEY_COUNT)
            return false;
        return keysDown[key];
    }

    bool Input::wasKeyPressed(int key)
    {
        if(key < 0 || key >= KEY_COUNT)
            return false;
        return keysPressed[key];
    }

    glm::vec2 Input::getMouseDelta()
    {
        return mouseDelta;
    }

    void Input::setCursorCaptured(bool captured)
    {
        cursorCaptured = captured;
        firstMouse = true;

        if(captured)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    bool Input::isCursorCaptured()
    {
        return cursorCaptured;
    }

    bool Input::wasResized()
    {
        return resized;
    }

    int Input::getFramebufferWidth()
    {
        return framebufferWidth;
    }

    int Input::getFramebufferHeight()
    {
        return framebufferHeight;
    }

    void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Input* input = (Input*)glfwGetWindowUserPointer(window);

        if(key < 0 || key >= KEY_COUNT)
            return;

        if(action == GLFW_PRESS)
        {
            input->keysDown[key] = true;
            input->keysPressed[key] = true;
        }
        else if(action == GLFW_RELEASE)
            input->keysDown[key] = false;
    }

    void Input::cursorCallback(GLFWwindow* window, double xpos, double ypos)
    {
        Input* input = (Input*)glfwGetWindowUserPointer(window);

        if(!input->cursorCaptured)
            return;

        if(input->firstMouse)
        {
            input->lastMouseX = xpos;
            input->lastMouseY = ypos;
            input->firstMouse = false;
            return;
        }

        input->mouseDelta.x += (float)(xpos - input->lastMouseX);
        input->mouseDelta.y += (float)(input->lastMouseY - ypos);

        input->lastMouseX = xpos;
        input->lastMouseY = ypos;
    }

    void Input::framebufferCallback(GLFWwindow* window, int width, int height)
    {
        Input* input = (Input*)glfwGetWindowUserPointer(window);

        if(width == 0 || height == 0)
            return;

        input->framebufferWidth = width;
        input->framebufferHeight = height;
        input->resized = true;
    }
}
