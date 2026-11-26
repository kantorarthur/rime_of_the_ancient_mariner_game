#ifndef Story_hpp
#define Story_hpp

#include "graphics/TextRenderer.hpp"

#include <string>
#include <vector>

namespace gps {

    enum class Chapter {Prologue, Departure, Storm};

    struct Caption
    {
        std::vector<std::string> lines;
        float duration;
        bool large;
    };

    class Story
    {
    public:
        void start();
        void update(float deltaTime, float distanceFromHome);
        void draw(gps::TextRenderer& text, int width, int height);

        Chapter getChapter();
        bool isShowingCaption();

    private:
        Chapter chapter = Chapter::Prologue;
        std::vector<Caption> captions;
        size_t current = 0;
        float captionTime = 0.0f;

        void say(std::vector<std::string> lines, float duration, bool large);
        void drawCaption(gps::TextRenderer& text, int width, int height, Caption& caption, float alpha);
    };
}

#endif
