#include "game/Story.hpp"

#include <glm/glm.hpp>

namespace gps {

    const float DEPARTURE_DISTANCE = 150.0f;
    const float STORM_LENGTH = 20.0f;
    const float MOURNING_LENGTH = 6.0f;
    const float HOME_DISTANCE = 130.0f;
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

    void Story::blessed()
    {
        if(chapter != Chapter::Blessing)
            return;

        enter(Chapter::Blessed);
        say({"And I blessed them unaware:", "Sure my kind saint took pity on me,", "And I blessed them unaware."}, 7.0f, false, "blessed");
        say({"The self-same moment I could pray;", "And from my neck so free", "The Albatross fell off, and sank", "Like lead into the sea."}, 9.0f, false, "albatrossfalls");
        say({"Oh sleep! it is a gentle thing,", "Beloved from pole to pole!"}, 6.0f, false);
        say({"And soon I heard a roaring wind:", "It did not come anear;", "But with its sound it shook the sails,", "That were so thin and sere."}, 9.0f, false, "rain");
        say({"The upper air burst into life!", "And a hundred fire-flags sheen,", "To and fro they were hurried about!", "And to and fro, and in and out,", "The wan stars danced between."}, 10.0f, false, "lightning");
        say({"They groaned, they stirred, they all uprose,", "Nor spake, nor moved their eyes;", "It had been strange, even in a dream,", "To have seen those dead men rise."}, 10.0f, false, "rise");
        say({"The helmsman steered, the ship moved on;", "Yet never a breeze up-blew;", "The mariners all 'gan work the ropes,", "Where they were wont to do;"}, 10.0f, false, "spiritwind");
    }

    void Story::boatArrived()
    {
        if(chapter != Chapter::Rescue)
            return;

        enter(Chapter::Returning);
        say({"Upon the whirl, where sank the ship,", "The boat spun round and round;", "And all was still, save that the hill", "Was telling of the sound."}, 9.0f, false, "aboard");
        say({"I moved my lips - the Pilot shrieked", "And fell down in a fit;", "The holy Hermit raised his eyes,", "And prayed where he did sit."}, 10.0f, false, "pilotfit");
        say({"I took the oars: the Pilot's boy,", "Who now doth crazy go,", "Laughed loud and long, and all the while", "His eyes went to and fro."}, 10.0f, false, "boylaughs");
    }

    void Story::landed()
    {
        if(chapter != Chapter::Returning || isShowingCaption())
            return;

        enter(Chapter::Ashore);
        say({"And now, all in my own countree,", "I stood on the firm land!"}, 8.0f, false, "ashore");
    }

    void Story::beginEpilogue()
    {
        if(chapter != Chapter::Interlude)
            return;

        enter(Chapter::Epilogue);
        say({"It is an ancient Mariner,", "And he stoppeth one of three.", "'By thy long grey beard and glittering eye,", "Now wherefore stopp'st thou me?"}, 10.0f, false, "wedding");
        say({"The Bridegroom's doors are opened wide,", "And I am next of kin;", "The guests are met, the feast is set:", "May'st hear the merry din.'"}, 10.0f, false, "doors");
        say({"The Bride hath paced into the hall,", "Red as a rose is she;", "Nodding their heads before her goes", "The merry minstrelsy."}, 10.0f, false, "bride");
        say({"He holds him with his glittering eye-", "The Wedding-Guest stood still,", "And listens like a three years' child:", "The Mariner hath his will."}, 10.0f, false, "listen");
        say({"O Wedding-Guest! this soul hath been", "Alone on a wide wide sea:", "So lonely 'twas, that God himself", "Scarce seemed there to be."}, 10.0f, false, "alonesea");
        say({"Farewell, farewell! but this I tell", "To thee, thou Wedding-Guest!", "He prayeth well, who loveth well", "Both man and bird and beast."}, 10.0f, false, "farewell");
        say({"He prayeth best, who loveth best", "All things both great and small;", "For the dear God who loveth us,", "He made and loveth all."}, 10.0f, false, "loveth");
        say({"The Mariner, whose eye is bright,", "Whose beard with age is hoar,", "Is gone: and now the Wedding-Guest", "Turned from the bridegroom's door."}, 11.0f, false, "gone");
        say({"He went like one that hath been stunned,", "And is of sense forlorn:", "A sadder and a wiser man,", "He rose the morrow morn."}, 11.0f, false, "wiser");
    }

    float Story::getChapterTime()
    {
        return chapterTime;
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
        else if(chapter == Chapter::Alone && chapterTime > 3.0f)
        {
            enter(Chapter::Snakes);
            say({"The many men, so beautiful!", "And they all dead did lie:", "And a thousand thousand slimy things", "Lived on; and so did I."}, 10.0f, false);
            say({"The moving Moon went up the sky,", "And no where did abide:", "Softly she was going up,", "And a star or two beside-"}, 9.0f, false);
            say({"Beyond the shadow of the ship,", "I watched the water-snakes:", "They moved in tracks of shining white,", "And when they reared, the elfish light", "Fell off in hoary flakes."}, 11.0f, false, "snakes");
            say({"Within the shadow of the ship", "I watched their rich attire:", "Blue, glossy green, and velvet black,", "They coiled and swam; and every track", "Was a flash of golden fire."}, 11.0f, false);
            say({"O happy living things! no tongue", "Their beauty might declare:", "A spring of love gushed from my heart,"}, 9.0f, false);
        }
        else if(chapter == Chapter::Snakes && !isShowingCaption())
            enter(Chapter::Blessing);
        else if(chapter == Chapter::Blessed && !isShowingCaption())
            enter(Chapter::Homeward);
        else if(chapter == Chapter::Homeward && distanceFromHome < HOME_DISTANCE)
        {
            enter(Chapter::Home);
            say({"Oh! dream of joy! is this indeed", "The light-house top I see?", "Is this the hill? is this the kirk?", "Is this mine own countree?"}, 10.0f, false, "home");
            say({"Each corse lay flat, lifeless and flat,", "And, by the holy rood!", "A man all light, a seraph-man,", "On every corse there stood."}, 10.0f, false, "seraphs");
            say({"Under the water it rumbled on,", "Still louder and more dread:", "It reached the ship, it split the bay;", "The ship went down like lead."}, 10.0f, false, "sink");
            say({"Like one that hath been seven days drowned", "My body lay afloat;", "But swift as dreams, myself I found", "Within the Pilot's boat."}, 10.0f, false, "afloat");
        }
        else if(chapter == Chapter::Home && !isShowingCaption())
            enter(Chapter::Rescue);
        else if(chapter == Chapter::Ashore && !isShowingCaption() && chapterTime > 9.0f)
            enter(Chapter::Interlude);
        else if(chapter == Chapter::Epilogue && !isShowingCaption())
            enter(Chapter::End);
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
