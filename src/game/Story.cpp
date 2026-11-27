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

    void Story::say(std::vector<std::string> lines, float duration, bool large, std::string event)
    {
        Caption caption = {lines, duration, large};
        caption.event = event;
        captions.push_back(caption);
    }

    void Story::shout(std::string line, float duration, std::string event)
    {
        Caption caption = {{line}, duration, false};
        caption.speech = true;
        caption.event = event;
        captions.push_back(caption);
    }

    std::vector<std::string> Story::takeEvents()
    {
        std::vector<std::string> taken = events;
        events.clear();
        return taken;
    }

    void Story::skipCaption()
    {
        if(isShowingCaption() && captionTime < captions[current].duration - 1.2f)
            captionTime = captions[current].duration - 1.2f;
    }

    void Story::ghostArrived()
    {
        if(chapter != Chapter::GhostApproach)
            return;

        enter(Chapter::Dice);
        say({"Are those her ribs through which the Sun", "Did peer, as through a grate?", "And is that Woman all her crew?", "Is that a DEATH? and are there two?", "Is DEATH that woman's mate?"}, 11.0f, false, "twain");
        say({"Her lips were red, her looks were free,", "Her locks were yellow as gold:", "Her skin was as white as leprosy,", "The Night-mare LIFE-IN-DEATH was she,", "Who thicks man's blood with cold."}, 11.0f, false, "lifeface");
        say({"The naked hulk alongside came,", "And the twain were casting dice;"}, 7.0f, false, "dice");
        shout("'The game is done! I've won! I've won!'", 4.0f, "won");
        say({"Quoth she, and whistles thrice."}, 4.0f, false);
        say({"The Sun's rim dips; the stars rush out:", "At one stride comes the dark;", "With far-heard whisper, o'er the sea,", "Off shot the spectre-bark."}, 10.0f, false, "ghostleaves");
        say({"One after one, by the star-dogged Moon,", "Too quick for groan or sigh,", "Each turned his face with a ghastly pang,", "And cursed me with his eye."}, 10.0f, false, "cursed");
        say({"Four times fifty living men,", "(And I heard nor sigh nor groan)", "With heavy thump, a lifeless lump,", "They dropped down one by one."}, 11.0f, false, "deaths");
        say({"The souls did from their bodies fly,-", "They fled to bliss or woe!", "And every soul, it passed me by,", "Like the whizz of my cross-bow!"}, 10.0f, false);
        say({"Alone, alone, all, all alone,", "Alone on a wide wide sea!", "And never a saint took pity on", "My soul in agony."}, 11.0f, false, "alone");
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
        captionStarted = false;
        say({"'God save thee, ancient Mariner!", "From the fiends, that plague thee thus!-", "Why look'st thou so?' - With my cross-bow", "I shot the ALBATROSS."}, 8.0f, false);
        shout("WHAT HAVE YOU DONE?", 4.0f);
        say({"And I had done a hellish thing,", "And it would work 'em woe:", "For all averred, I had killed the bird", "That made the breeze to blow."}, 9.0f, false);
        say({"Ah wretch! said they, the bird to slay,", "That made the breeze to blow!"}, 7.0f, false);
    }

    void Story::update(float deltaTime, float distanceFromHome)
    {
        chapterTime += deltaTime;

        if(current < captions.size() && !captionStarted)
        {
            captionStarted = true;
            if(!captions[current].event.empty())
                events.push_back(captions[current].event);
        }

        if(current < captions.size())
        {
            captionTime += deltaTime;
            if(captionTime >= captions[current].duration)
            {
                current++;
                captionTime = 0.0f;
                captionStarted = false;
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
            say({"The very deep did rot: O Christ!", "That ever this should be!", "Yea, slimy things did crawl with legs", "Upon the slimy sea."}, 10.0f, false, "deathfires");
            say({"About, about, in reel and rout", "The death-fires danced at night;", "The water, like a witch's oils,", "Burnt green, and blue and white."}, 10.0f, false);
            say({"Ah! well a-day! what evil looks", "Had I from old and young!", "Instead of the cross, the Albatross", "About my neck was hung."}, 10.0f, false, "neck");
            say({"With throats unslaked, with black lips baked,", "We could nor laugh nor wail;", "Through utter drought all dumb we stood!", "I bit my arm, I sucked the blood,", "And cried, A sail! a sail!"}, 11.0f, false, "sail");
            say({"See! see! (I cried) she tacks no more!", "Hither to work us weal;", "Without a breeze, without a tide,", "She steadies with upright keel!"}, 10.0f, false);
        }
        else if(chapter == Chapter::Becalmed && !isShowingCaption())
            enter(Chapter::GhostApproach);
        else if(chapter == Chapter::Dice && !isShowingCaption())
            enter(Chapter::Alone);
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
