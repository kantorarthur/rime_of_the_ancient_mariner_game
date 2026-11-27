#include "game/PauseMenu.hpp"

#include <glm/glm.hpp>

namespace gps {

    const int MENU_ITEMS = 4;
    const int RESUME_ITEM = 0;
    const int WINDOW_ITEM = 1;
    const int CONTROLS_ITEM = 2;
    const int EXIT_ITEM = 3;

    const glm::vec4 DIM_COLOR = glm::vec4(0.0f, 0.0f, 0.0f, 0.7f);
    const glm::vec4 TITLE_COLOR = glm::vec4(0.95f, 0.9f, 0.8f, 1.0f);
    const glm::vec4 ITEM_COLOR = glm::vec4(0.7f, 0.72f, 0.75f, 1.0f);
    const glm::vec4 SELECTED_COLOR = glm::vec4(1.0f, 0.82f, 0.5f, 1.0f);
    const glm::vec4 FOOTER_COLOR = glm::vec4(0.6f, 0.62f, 0.65f, 1.0f);

    const char* CONTROL_KEYS[] = {
        "W / S", "A / D", "Mouse", "X", "B", "C", "Enter",
        "W A S D", "Space / Left Ctrl", "Left Shift", "Mouse wheel", "Z",
        "Tab", "1 / 2 / 3", "4", "T", "F", "L", "Esc"
    };

    const char* CONTROL_ACTIONS[] = {
        "raise / lower the sails",
        "turn the helm left / right",
        "look around",
        "take up the cross-bow (when the story asks for it)",
        "bless the water-snakes (when the story asks for it)",
        "switch between the ship and the free camera",
        "skip to the next verse",
        "move the free camera",
        "free camera up / down",
        "hold to move the free camera faster",
        "zoom in / out",
        "reset the zoom",
        "release / capture the mouse",
        "solid / wireframe / points",
        "flat / smooth shading",
        "calm / storm",
        "fog on / off",
        "lightning strike (during the storm)",
        "this menu"
    };

    const int CONTROL_COUNT = sizeof(CONTROL_KEYS) / sizeof(CONTROL_KEYS[0]);

    void PauseMenu::update(gps::Input& input, gps::Window& window)
    {
        if(!opened)
        {
            if(input.wasKeyPressed(GLFW_KEY_ESCAPE))
                open(input);
            return;
        }

        bool back = input.wasKeyPressed(GLFW_KEY_ESCAPE) || input.wasKeyPressed(GLFW_KEY_BACKSPACE);
        bool accept = input.wasKeyPressed(GLFW_KEY_ENTER) || input.wasKeyPressed(GLFW_KEY_KP_ENTER) || input.wasKeyPressed(GLFW_KEY_SPACE);

        if(page == Page::Controls)
        {
            if(back || accept)
                page = Page::Main;
            return;
        }

        if(back)
        {
            close(input);
            return;
        }

        if(input.wasKeyPressed(GLFW_KEY_UP) || input.wasKeyPressed(GLFW_KEY_W))
            selected = (selected + MENU_ITEMS - 1) % MENU_ITEMS;
        if(input.wasKeyPressed(GLFW_KEY_DOWN) || input.wasKeyPressed(GLFW_KEY_S))
            selected = (selected + 1) % MENU_ITEMS;

        if(selected == WINDOW_ITEM)
        {
            if(input.wasKeyPressed(GLFW_KEY_LEFT) || input.wasKeyPressed(GLFW_KEY_A))
                changeWindowMode(window, -1);
            if(input.wasKeyPressed(GLFW_KEY_RIGHT) || input.wasKeyPressed(GLFW_KEY_D))
                changeWindowMode(window, 1);
        }

        if(!accept)
            return;

        if(selected == RESUME_ITEM)
            close(input);
        else if(selected == WINDOW_ITEM)
            changeWindowMode(window, 1);
        else if(selected == CONTROLS_ITEM)
            page = Page::Controls;
        else if(selected == EXIT_ITEM)
            quit = true;
    }

    bool PauseMenu::isOpen()
    {
        return opened;
    }

    bool PauseMenu::wantsToQuit()
    {
        return quit;
    }

    void PauseMenu::open(gps::Input& input)
    {
        opened = true;
        page = Page::Main;
        selected = RESUME_ITEM;
        cursorWasCaptured = input.isCursorCaptured();
        input.setCursorCaptured(false);
    }

    void PauseMenu::close(gps::Input& input)
    {
        opened = false;
        input.setCursorCaptured(cursorWasCaptured);
    }

    void PauseMenu::changeWindowMode(gps::Window& window, int step)
    {
        int mode = ((int)window.getMode() + step + 3) % 3;
        window.setMode((gps::WindowMode)mode);
    }

    std::string PauseMenu::describeMode(gps::WindowMode mode)
    {
        if(mode == gps::WindowMode::Borderless)
            return "Borderless window";
        if(mode == gps::WindowMode::Fullscreen)
            return "Fullscreen";
        return "Windowed";
    }

    void PauseMenu::draw(gps::TextRenderer& text, gps::Window& window, int width, int height)
    {
        if(!opened)
            return;

        text.begin(width, height);
        text.drawRectangle(0.0f, 0.0f, (float)width, (float)height, DIM_COLOR);
        if(page == Page::Main)
            drawMain(text, window, width, height);
        else
            drawControls(text, width, height);
        text.end();
    }

    void PauseMenu::drawMain(gps::TextRenderer& text, gps::Window& window, int width, int height)
    {
        float titleScale = height / 180.0f;
        float itemScale = height / 300.0f;
        float footerScale = height / 600.0f;
        float centerX = width * 0.5f;

        text.drawTextCentered("Paused", centerX, height * 0.22f, titleScale, TITLE_COLOR);

        std::string items[MENU_ITEMS] = {
            "Resume game",
            "Window:   <  " + describeMode(window.getMode()) + "  >",
            "Controls",
            "Exit game"
        };

        float y = height * 0.4f;
        for(int i = 0; i < MENU_ITEMS; i++)
        {
            glm::vec4 color = i == selected ? SELECTED_COLOR : ITEM_COLOR;
            std::string label = i == selected ? "-  " + items[i] + "  -" : items[i];
            text.drawTextCentered(label, centerX, y, itemScale, color);
            y += text.lineHeight(itemScale) * 1.8f;
        }

        text.drawTextCentered("Up/Down  choose     Left/Right  change window     Enter  select     Esc  resume", centerX, height * 0.88f, footerScale, FOOTER_COLOR);
    }

    void PauseMenu::drawControls(gps::TextRenderer& text, int width, int height)
    {
        float titleScale = height / 220.0f;
        float scale = height / 480.0f;
        float footerScale = height / 600.0f;

        text.drawTextCentered("Controls", width * 0.5f, height * 0.1f, titleScale, TITLE_COLOR);

        float keyWidth = 0.0f;
        float actionWidth = 0.0f;
        for(int i = 0; i < CONTROL_COUNT; i++)
        {
            keyWidth = glm::max(keyWidth, text.measure(CONTROL_KEYS[i], scale));
            actionWidth = glm::max(actionWidth, text.measure(CONTROL_ACTIONS[i], scale));
        }

        float gap = text.lineHeight(scale) * 2.0f;
        float left = (width - keyWidth - gap - actionWidth) * 0.5f;
        float y = height * 0.2f;
        for(int i = 0; i < CONTROL_COUNT; i++)
        {
            text.drawText(CONTROL_KEYS[i], left, y, scale, SELECTED_COLOR);
            text.drawText(CONTROL_ACTIONS[i], left + keyWidth + gap, y, scale, ITEM_COLOR);
            y += text.lineHeight(scale) * 1.45f;
        }

        text.drawTextCentered("Esc / Enter  back", width * 0.5f, height * 0.93f, footerScale, FOOTER_COLOR);
    }
}
