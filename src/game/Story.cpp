#include "game/Story.hpp"

#include <glm/glm.hpp>

namespace gps {

    const float DEPARTURE_DISTANCE = 150.0f;
    const float STORM_LENGTH = 20.0f;
    const float MOURNING_LENGTH = 6.0f;
    const float FADE_TIME = 1.2f;
    const glm::vec3 VERSE_COLOR = glm::vec3(0.93f, 0.88f, 0.76f);
    const glm::vec3 SPEECH_COLOR = glm::vec3(0.95f, 0.55f, 0.45f);

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

    void Story::shout(std::string line, float duration)
    {
        Caption caption = {{line}, duration, false};
        caption.speech = true;
        captions.push_back(caption);
    }

    void Story::enter(Chapter next)
    {
        chapter = next;
        chapterTime = 0.0f;
    }

    void Story::albatrossShot()
    {
        if(chapter != Chapter::Crossbow)
            return;

        enter(Chapter::Shot);
        captions.erase(captions.begin() + current, captions.end());
        captionTime = 0.0f;
        say({"'God save thee, ancient Mariner!", "From the fiends, that plague thee thus!-", "Why look'st thou so?' - With my cross-bow", "I shot the ALBATROSS."}, 8.0f, false);
        shout("WHAT HAVE YOU DONE?", 4.0f);
        say({"And I had done a hellish thing,", "And it would work 'em woe:", "For all averred, I had killed the bird", "That made the breeze to blow."}, 9.0f, false);
        say({"Ah wretch! said they, the bird to slay,", "That made the breeze to blow!"}, 7.0f, false);
    }

    void Story::update(float deltaTime, float distanceFromHome)
    {
        chapterTime += deltaTime;

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
            enter(Chapter::Departure);

        if(chapter == Chapter::Departure && distanceFromHome > DEPARTURE_DISTANCE)
        {
            enter(Chapter::Storm);
            say({"And now the STORM-BLAST came, and he", "Was tyrannous and strong:", "He struck with his o'ertaking wings,", "And chased us south along."}, 9.0f, false);
            say({"With sloping masts and dipping prow,", "As who pursued with yell and blow", "Still treads the shadow of his foe,", "The ship drove fast, loud roared the blast,", "And southward aye we fled."}, 10.0f, false);
        }
        else if(chapter == Chapter::Storm && !isShowingCaption() && chapterTime > STORM_LENGTH)
        {
            enter(Chapter::Albatross);
            say({"At length did cross an Albatross,", "Thorough the fog it came;", "As if it had been a Christian soul,", "We hailed it in God's name."}, 10.0f, false);
            say({"It ate the food it ne'er had eat,", "And round and round it flew.", "The ice did split with a thunder-fit;", "The helmsman steered us through!"}, 9.0f, false);
            say({"And a good south wind sprung up behind;", "The Albatross did follow,", "And every day, for food or play,", "Came to the mariner's hollo!"}, 10.0f, false);
            say({"In mist or cloud, on mast or shroud,", "It perched for vespers nine;", "Whiles all the night, through fog-smoke white,", "Glimmered the white Moon-shine."}, 10.0f, false);
        }
        else if(chapter == Chapter::Albatross && !isShowingCaption())
            enter(Chapter::Crossbow);
        else if(chapter == Chapter::Shot && !isShowingCaption() && chapterTime > MOURNING_LENGTH)
        {
            enter(Chapter::Becalmed);
            say({"Down dropt the breeze, the sails dropt down,", "'Twas sad as sad could be;", "And we did speak only to break", "The silence of the sea!"}, 10.0f, false);
            say({"Day after day, day after day,", "We stuck, nor breath nor motion;", "As idle as a painted ship", "Upon a painted ocean."}, 10.0f, false);
            say({"Water, water, every where,", "And all the boards did shrink;", "Water, water, every where,", "Nor any drop to drink."}, 10.0f, false);
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
        float scale = height / 330.0f;
        if(caption.large || caption.speech)
            scale = height / 220.0f;
        float lineHeight = text.lineHeight(scale);
        float blockHeight = lineHeight * caption.lines.size();
        float top = caption.large ? (height - blockHeight) * 0.42f : height * 0.70f - blockHeight * 0.5f;

        float widest = 0.0f;
        for(std::string& line : caption.lines)
            widest = glm::max(widest, text.measure(line, scale));

        float padding = lineHeight * 0.8f;
        text.drawRectangle(width * 0.5f - widest * 0.5f - padding, top - padding, widest + 2.0f * padding, blockHeight + 1.5f * padding, glm::vec4(0.0f, 0.0f, 0.0f, 0.35f * alpha));

        for(size_t i = 0; i < caption.lines.size(); i++)
            text.drawTextCentered(caption.lines[i], width * 0.5f, top + i * lineHeight, scale, glm::vec4(caption.speech ? SPEECH_COLOR : VERSE_COLOR, alpha));
    }
}
