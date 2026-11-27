#ifndef Window_hpp
#define Window_hpp

#include "core/OpenGL.hpp"

namespace gps {

    struct WindowDimensions
    {
        int width;
        int height;
    };

    enum class WindowMode {Windowed, Borderless, Fullscreen};

    class Window
    {
    public:
        void Create(int width, int height, const char* title);
        void Delete();

        GLFWwindow* getWindow();
        WindowDimensions getWindowDimensions();
        void setWindowDimensions(WindowDimensions dimensions);
        void setMode(WindowMode newMode);
        WindowMode getMode();

    private:
        WindowDimensions dimensions = {0, 0};
        GLFWwindow* window = nullptr;
        WindowMode mode = WindowMode::Windowed;
        int windowedX = 100;
        int windowedY = 100;
        int windowedWidth = 1024;
        int windowedHeight = 768;
    };
}

#endif
