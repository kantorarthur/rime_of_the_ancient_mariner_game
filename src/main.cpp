#include "core/OpenGL.hpp"
#include "core/Window.hpp"
#include "core/Input.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Model3D.hpp"
#include "graphics/ViewMode.hpp"
#include "scene/Camera.hpp"
#include "scene/Scene.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <stdexcept>
#include <string>

gps::Window myWindow;
gps::Input input;
gps::ViewMode viewMode;

glm::mat4 view;
glm::mat4 projection;

glm::vec3 lightDir;
glm::vec3 lightColor;

GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;
GLint flatShadingLoc;

gps::Camera myCamera(
    glm::vec3(-22.0f, 7.0f, 62.0f),
    glm::vec3(0.0f, 5.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

float cameraSpeed = 8.0f;
float sprintMultiplier = 3.0f;
float mouseSensitivity = 0.1f;

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

gps::Scene scene;

gps::Shader myBasicShader;

GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch(errorCode)
        {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void updateProjection(int width, int height)
{
    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 300.0f);
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void handleResize()
{
    if(!input.wasResized())
        return;

    int width = input.getFramebufferWidth();
    int height = input.getFramebufferHeight();

    myWindow.setWindowDimensions({width, height});
    glViewport(0, 0, width, height);
    updateProjection(width, height);
}

void updateWindowTitle()
{
    std::string title = "The Rime of the Ancient Mariner | " + viewMode.describe();
    glfwSetWindowTitle(myWindow.getWindow(), title.c_str());
}

void handleKeyPresses()
{
    if(input.wasKeyPressed(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(myWindow.getWindow(), GL_TRUE);

    if(input.wasKeyPressed(GLFW_KEY_TAB))
        input.setCursorCaptured(!input.isCursorCaptured());

    bool viewChanged = false;

    if(input.wasKeyPressed(GLFW_KEY_1))
    {
        viewMode.setPolygonMode(gps::PolygonMode::Solid);
        viewChanged = true;
    }
    if(input.wasKeyPressed(GLFW_KEY_2))
    {
        viewMode.setPolygonMode(gps::PolygonMode::Wireframe);
        viewChanged = true;
    }
    if(input.wasKeyPressed(GLFW_KEY_3))
    {
        viewMode.setPolygonMode(gps::PolygonMode::Points);
        viewChanged = true;
    }
    if(input.wasKeyPressed(GLFW_KEY_4))
    {
        viewMode.toggleFlatShading();
        viewChanged = true;
    }

    if(viewChanged)
        updateWindowTitle();
}

void processCamera()
{
    glm::vec2 mouseDelta = input.getMouseDelta() * mouseSensitivity;
    myCamera.rotate(mouseDelta.y, mouseDelta.x);

    float velocity = cameraSpeed * deltaTime;
    if(input.isKeyDown(GLFW_KEY_LEFT_SHIFT))
        velocity *= sprintMultiplier;

    if(input.isKeyDown(GLFW_KEY_W))
        myCamera.move(gps::MOVE_FORWARD, velocity);
    if(input.isKeyDown(GLFW_KEY_S))
        myCamera.move(gps::MOVE_BACKWARD, velocity);
    if(input.isKeyDown(GLFW_KEY_A))
        myCamera.move(gps::MOVE_LEFT, velocity);
    if(input.isKeyDown(GLFW_KEY_D))
        myCamera.move(gps::MOVE_RIGHT, velocity);
    if(input.isKeyDown(GLFW_KEY_SPACE))
        myCamera.move(gps::MOVE_UP, velocity);
    if(input.isKeyDown(GLFW_KEY_LEFT_CONTROL))
        myCamera.move(gps::MOVE_DOWN, velocity);
}

void initOpenGLWindow()
{
    myWindow.Create(1024, 768, "The Rime of the Ancient Mariner");
    input.attach(myWindow.getWindow());
    updateWindowTitle();
}

void initOpenGLState()
{
    glClearColor(0.55f, 0.65f, 0.75f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void initModels()
{
    scene.load();
}

void initShaders()
{
    myBasicShader.loadShader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
}

void initUniforms()
{
    myBasicShader.useShaderProgram();

    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    flatShadingLoc = glGetUniformLocation(myBasicShader.shaderProgram, "flatShading");

    updateProjection(myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

    lightDir = glm::vec3(0.5f, 1.0f, 0.7f);
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
}

void renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    viewMode.apply();

    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniform1i(flatShadingLoc, viewMode.isFlatShading());

    scene.draw(myBasicShader, view, modelLoc, normalMatrixLoc);
}

void cleanup()
{
    myWindow.Delete();
}

int main()
{
    try
    {
        initOpenGLWindow();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initModels();
    initShaders();
    initUniforms();
    glCheckError();

    lastFrameTime = (float)glfwGetTime();

    while(!glfwWindowShouldClose(myWindow.getWindow()))
    {
        glfwPollEvents();

        float currentFrameTime = (float)glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
        if(deltaTime > 0.1f)
            deltaTime = 0.1f;

        handleResize();
        handleKeyPresses();
        processCamera();

        renderScene();
        glfwSwapBuffers(myWindow.getWindow());

        input.endFrame();
        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
