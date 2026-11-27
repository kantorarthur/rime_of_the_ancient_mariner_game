#include "core/OpenGL.hpp"
#include "core/Window.hpp"
#include "core/Input.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Model3D.hpp"
#include "graphics/ViewMode.hpp"
#include "game/Game.hpp"
#include "game/PauseMenu.hpp"
#include "graphics/TextRenderer.hpp"
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

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

gps::Scene scene;
gps::PauseMenu pauseMenu;
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
gps::Shader textShader;
gps::Shader skinnedShader;
gps::Shader skinnedDepthShader;
gps::Shader ghostShader;
gps::Shader soulShader;
gps::Shader snakeShader;

gps::Game game;
gps::TextRenderer textRenderer;

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
    std::string title = "The Rime of the Ancient Mariner | " + game.describeMode() + " | " + viewMode.describe() + " | " + weather + ", " + fog;
    glfwSetWindowTitle(myWindow.getWindow(), title.c_str());
}

void handleKeyPresses()
{
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
        viewChanged = true;
    }

    if(input.wasKeyPressed(GLFW_KEY_C))
        viewChanged = true;

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
    textShader.loadShader("assets/shaders/text.vert", "assets/shaders/text.frag");
    skinnedShader.loadShader("assets/shaders/skinned.vert", "assets/shaders/basic.frag");
    skinnedDepthShader.loadShader("assets/shaders/skinned_shadow.vert", "assets/shaders/shadow.frag");
    ghostShader.loadShader("assets/shaders/basic.vert", "assets/shaders/ghost.frag");
    soulShader.loadShader("assets/shaders/soul.vert", "assets/shaders/soul.frag");
    snakeShader.loadShader("assets/shaders/snake.vert", "assets/shaders/snake.frag");
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
    scene.renderShadows(depthShader, skinnedDepthShader);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    viewMode.apply();

    view = myCamera.getViewMatrix();
    glm::vec3 cameraPosition = myCamera.getPosition();

    scene.uploadEnvironment(myBasicShader, cameraPosition);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniform1i(flatShadingLoc, viewMode.isFlatShading());
    scene.draw(myBasicShader, modelLoc, normalMatrixLoc, emissiveLoc);
    skinnedShader.useShaderProgram();
    glUniform1i(glGetUniformLocation(skinnedShader.shaderProgram, "flatShading"), viewMode.isFlatShading());
    scene.drawCharacters(skinnedShader, view, projection, cameraPosition);
    scene.drawWater(waterShader, view, projection, cameraPosition, skybox.getSelectedTexture());

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if(scene.getWeather() == gps::Weather::Calm)
        skybox.select(calmSky);
    else
        skybox.select(stormSky);
    scene.uploadEnvironment(skyboxShader, cameraPosition);
    skybox.draw(skyboxShader, view, projection, 1.0f);
    scene.drawGhosts(ghostShader, view, projection, cameraPosition);
    scene.drawBeam(beamShader, view, projection, cameraPosition);
    scene.drawSnakes(snakeShader, view, projection, cameraPosition);
    scene.drawSouls(soulShader, view, projection, cameraPosition);
    scene.drawWeather(rainShader, boltShader, view, projection, cameraPosition);

    if(!pauseMenu.isOpen())
        game.drawOverlay(textRenderer, scene, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    pauseMenu.draw(textRenderer, myWindow, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
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
    textRenderer.init(textShader);
    game.start(scene);
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
        bool wasPaused = pauseMenu.isOpen();
        pauseMenu.update(input, myWindow);
        if(pauseMenu.wantsToQuit())
            glfwSetWindowShouldClose(myWindow.getWindow(), GL_TRUE);
        if(!wasPaused && !pauseMenu.isOpen())
        {
            game.update(deltaTime, input, myCamera, scene);
            handleKeyPresses();
            scene.update(deltaTime, myCamera.getPosition(), myCamera.getFrontDirection());
        }

        renderScene();
        glfwSwapBuffers(myWindow.getWindow());

        input.endFrame();
        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
