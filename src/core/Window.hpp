#ifndef Window_hpp
#define Window_hpp

#include "core/OpenGL.hpp"

namespace gps {

    struct WindowDimensions
    {
        int width;
        int height;
    };

    class Window
    {
    public:
        void Create(int width, int height, const char* title);
        void Delete();

        GLFWwindow* getWindow();
        WindowDimensions getWindowDimensions();
        void setWindowDimensions(WindowDimensions dimensions);

    private:
        WindowDimensions dimensions = {0, 0};
        GLFWwindow* window = nullptr;
    };
}

#endif
