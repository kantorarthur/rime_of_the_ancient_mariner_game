#ifndef Story_hpp
#define Story_hpp

#include "graphics/TextRenderer.hpp"

#include <string>
#include <vector>

namespace gps {

    enum class Chapter {Prologue, Departure, Storm, Albatross, Crossbow, Shot, Becalmed};

    struct Caption
    {
        std::vector<std::string> lines;
        float duration;
        bool large;
        bool speech = false;
    };

    class Story
    {
    public:
        void start();
        void update(float deltaTime, float distanceFromHome);
        void albatrossShot();
        void draw(gps::TextRenderer& text, int width, int height);

        Chapter getChapter();
        bool isShowingCaption();

    private:
        Chapter chapter = Chapter::Prologue;
        std::vector<Caption> captions;
        size_t current = 0;
        float captionTime = 0.0f;
        float chapterTime = 0.0f;

        void say(std::vector<std::string> lines, float duration, bool large);
        void shout(std::string line, float duration);
        void enter(Chapter next);
        void drawCaption(gps::TextRenderer& text, int width, int height, Caption& caption, float alpha);
    };
}

#endif
