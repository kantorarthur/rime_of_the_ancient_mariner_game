#ifndef PauseMenu_hpp
#define PauseMenu_hpp

#include "core/Input.hpp"
#include "core/Window.hpp"
#include "graphics/TextRenderer.hpp"

#include <string>

namespace gps {

    class PauseMenu
    {
    public:
        void update(gps::Input& input, gps::Window& window);
        void draw(gps::TextRenderer& text, gps::Window& window, int width, int height);
        bool isOpen();
        bool wantsToQuit();

    private:
        enum class Page {Main, Controls};

        bool opened = false;
        bool quit = false;
        bool cursorWasCaptured = true;
        Page page = Page::Main;
        int selected = 0;

        void open(gps::Input& input);
        void close(gps::Input& input);
        void changeWindowMode(gps::Window& window, int step);
        std::string describeMode(gps::WindowMode mode);
        void drawMain(gps::TextRenderer& text, gps::Window& window, int width, int height);
        void drawControls(gps::TextRenderer& text, int width, int height);
    };
}

#endif
