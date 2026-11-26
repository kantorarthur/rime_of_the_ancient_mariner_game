#include "game/Story.hpp"

#include <glm/glm.hpp>

namespace gps {

    const float DEPARTURE_DISTANCE = 150.0f;
    const float FADE_TIME = 1.2f;
    const glm::vec3 VERSE_COLOR = glm::vec3(0.93f, 0.88f, 0.76f);

    void Story::start()
    {
        chapter = Chapter::Prologue;
        captions.clear();
        current = 0;
        captionTime = 0.0f;

        say({"THE RIME OF THE ANCIENT MARINER", "", "Samuel Taylor Coleridge, 1798"}, 6.0f, true);
        say({"It is an ancient Mariner,", "And he stoppeth one of three.", "'By thy long grey beard and glittering eye,", "Now wherefore stopp'st thou me?'"}, 8.0f, false);
        say({"The ship was cheered, the harbour cleared,", "Merrily did we drop", "Below the kirk, below the hill,", "Below the lighthouse top."}, 8.0f, false);
    }

    void Story::say(std::vector<std::string> lines, float duration, bool large)
    {
        captions.push_back({lines, duration, large});
    }

    void Story::update(float deltaTime, float distanceFromHome)
    {
        if(current < captions.size())
        {
            captionTime += deltaTime;
            if(captionTime >= captions[current].duration)
            {
                current++;
                captionTime = 0.0f;
            }
        }

        if(chapter == Chapter::Prologue && !isShowingCaption())
            chapter = Chapter::Departure;

        if(chapter == Chapter::Departure && distanceFromHome > DEPARTURE_DISTANCE)
        {
            chapter = Chapter::Storm;
            say({"And now the STORM-BLAST came, and he", "Was tyrannous and strong:", "He struck with his o'ertaking wings,", "And chased us south along."}, 9.0f, false);
            say({"With sloping masts and dipping prow,", "As who pursued with yell and blow", "Still treads the shadow of his foe,", "The ship drove fast, loud roared the blast,", "And southward aye we fled."}, 10.0f, false);
        }
    }

    bool Story::isShowingCaption()
    {
        return current < captions.size();
    }

    Chapter Story::getChapter()
    {
        return chapter;
    }

    void Story::draw(gps::TextRenderer& text, int width, int height)
    {
        if(!isShowingCaption())
            return;

        Caption& caption = captions[current];
        float fadeIn = glm::smoothstep(0.0f, FADE_TIME, captionTime);
        float fadeOut = 1.0f - glm::smoothstep(caption.duration - FADE_TIME, caption.duration, captionTime);
        drawCaption(text, width, height, caption, fadeIn * fadeOut);
    }

    void Story::drawCaption(gps::TextRenderer& text, int width, int height, Caption& caption, float alpha)
    {
        float scale = caption.large ? height / 220.0f : height / 330.0f;
        float lineHeight = text.lineHeight(scale);
        float blockHeight = lineHeight * caption.lines.size();
        float top = caption.large ? (height - blockHeight) * 0.42f : height * 0.70f - blockHeight * 0.5f;

        float widest = 0.0f;
        for(std::string& line : caption.lines)
            widest = glm::max(widest, text.measure(line, scale));

        float padding = lineHeight * 0.8f;
        text.drawRectangle(width * 0.5f - widest * 0.5f - padding, top - padding, widest + 2.0f * padding, blockHeight + 1.5f * padding, glm::vec4(0.0f, 0.0f, 0.0f, 0.35f * alpha));

        for(size_t i = 0; i < caption.lines.size(); i++)
            text.drawTextCentered(caption.lines[i], width * 0.5f, top + i * lineHeight, scale, glm::vec4(VERSE_COLOR, alpha));
    }
}
