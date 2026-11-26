#include "core/OpenGL.hpp"
#include "core/Window.hpp"
#include "core/Input.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Model3D.hpp"
#include "graphics/ViewMode.hpp"
#include "scene/Camera.hpp"
#include "scene/Scene.hpp"
#include "scene/Skybox.hpp"

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

GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint flatShadingLoc;
GLint emissiveLoc;

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
gps::Skybox skybox;
int calmSky = 0;
int stormSky = 0;

gps::Shader myBasicShader;
gps::Shader skyboxShader;
gps::Shader waterShader;
gps::Shader beamShader;
gps::Shader depthShader;
gps::Shader rainShader;
gps::Shader boltShader;

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
    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.2f, 2000.0f);
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
    std::string weather = scene.getWeather() == gps::Weather::Calm ? "calm" : "storm";
    std::string fog = scene.isFogEnabled() ? "fog" : "clear";
    std::string title = "The Rime of the Ancient Mariner | " + viewMode.describe() + " | " + weather + ", " + fog;
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

    if(input.wasKeyPressed(GLFW_KEY_T))
    {
        scene.toggleWeather();
        if(scene.getWeather() == gps::Weather::Calm)
            skybox.select(calmSky);
        else
            skybox.select(stormSky);
        viewChanged = true;
    }

    if(input.wasKeyPressed(GLFW_KEY_L))
        scene.triggerLightning(myCamera.getPosition(), myCamera.getFrontDirection());

    if(input.wasKeyPressed(GLFW_KEY_F))
    {
        scene.toggleFog();
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
}

void initOpenGLState()
{
    glClearColor(0.02f, 0.03f, 0.05f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);
}

void initModels()
{
    scene.load();

    skybox.init();
    calmSky = skybox.addCubemap("assets/skybox/calm");
    stormSky = skybox.addCubemap("assets/skybox/storm");
    if(scene.getWeather() == gps::Weather::Calm)
        skybox.select(calmSky);
    else
        skybox.select(stormSky);
}

void initShaders()
{
    myBasicShader.loadShader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    skyboxShader.loadShader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
    waterShader.loadShader("assets/shaders/water.vert", "assets/shaders/water.frag");
    beamShader.loadShader("assets/shaders/beam.vert", "assets/shaders/beam.frag");
    depthShader.loadShader("assets/shaders/shadow.vert", "assets/shaders/shadow.frag");
    rainShader.loadShader("assets/shaders/rain.vert", "assets/shaders/rain.frag");
    boltShader.loadShader("assets/shaders/bolt.vert", "assets/shaders/bolt.frag");
}

void initUniforms()
{
    myBasicShader.useShaderProgram();

    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");
    flatShadingLoc = glGetUniformLocation(myBasicShader.shaderProgram, "flatShading");
    emissiveLoc = glGetUniformLocation(myBasicShader.shaderProgram, "emissiveStrength");

    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "shininess"), 32.0f);

    updateProjection(myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
}

void renderScene()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    scene.renderShadows(depthShader);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    viewMode.apply();

    view = myCamera.getViewMatrix();
    glm::vec3 cameraPosition = myCamera.getPosition();

    scene.uploadEnvironment(myBasicShader, cameraPosition);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniform1i(flatShadingLoc, viewMode.isFlatShading());
    scene.draw(myBasicShader, modelLoc, normalMatrixLoc, emissiveLoc);
    scene.drawWater(waterShader, view, projection, cameraPosition, skybox.getSelectedTexture());

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    scene.uploadEnvironment(skyboxShader, cameraPosition);
    skybox.draw(skyboxShader, view, projection, 1.0f);
    scene.drawBeam(beamShader, view, projection, cameraPosition);
    scene.drawWeather(rainShader, boltShader, view, projection, cameraPosition);
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
    updateWindowTitle();
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
        scene.update(deltaTime, myCamera.getPosition(), myCamera.getFrontDirection());

        renderScene();
        glfwSwapBuffers(myWindow.getWindow());

        input.endFrame();
        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
