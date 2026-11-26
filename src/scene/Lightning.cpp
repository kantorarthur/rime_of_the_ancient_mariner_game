#include "scene/Lightning.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <cstddef>

namespace gps {

    struct BoltVertex
    {
        glm::vec3 position;
        float across;
    };

    const float BOLT_DISTANCE_MIN = 550.0f;
    const float BOLT_DISTANCE_MAX = 850.0f;
    const float BOLT_TOP_HEIGHT = 380.0f;
    const float BOLT_WIDTH = 4.5f;
    const int BOLT_DEPTH = 7;
    const float BOLT_VISIBLE_TIME = 0.55f;
    const float STRIKE_INTERVAL_MIN = 5.0f;
    const float STRIKE_INTERVAL_MAX = 14.0f;
    const float FLASH_DECAY = 14.0f;

    void Lightning::init()
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BoltVertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(BoltVertex), (void*)offsetof(BoltVertex, across));
        glBindVertexArray(0);
    }

    float Lightning::randomRange(float low, float high)
    {
        std::uniform_real_distribution<float> distribution(low, high);
        return distribution(random);
    }

    void Lightning::addBolt(glm::vec3 start, glm::vec3 end, float width, int depth, bool canBranch)
    {
        if(depth == 0)
        {
            segments.push_back({start, end, width});
            return;
        }

        float length = glm::length(end - start);
        glm::vec3 jitter = glm::vec3(randomRange(-1.0f, 1.0f), randomRange(-0.3f, 0.3f), randomRange(-1.0f, 1.0f));
        glm::vec3 middle = (start + end) * 0.5f + jitter * length * 0.22f;

        addBolt(start, middle, width, depth - 1, canBranch);
        addBolt(middle, end, width, depth - 1, canBranch);

        if(canBranch && depth >= 3 && randomRange(0.0f, 1.0f) < 0.35f)
        {
            glm::vec3 sideways = glm::vec3(randomRange(-1.0f, 1.0f), 0.0f, randomRange(-1.0f, 1.0f));
            glm::vec3 branchEnd = middle + glm::normalize(end - start + sideways * length) * length * 0.7f;
            addBolt(middle, branchEnd, width * 0.5f, depth - 2, false);
        }
    }

    void Lightning::strike(glm::vec3 cameraPosition, glm::vec3 viewDirection)
    {
        glm::vec2 forward = glm::vec2(viewDirection.x, viewDirection.z);
        if(glm::length(forward) < 0.01f)
            forward = glm::vec2(1.0f, 0.0f);
        float angle = std::atan2(forward.y, forward.x) + glm::radians(randomRange(-35.0f, 35.0f));
        float distance = randomRange(BOLT_DISTANCE_MIN, BOLT_DISTANCE_MAX);

        glm::vec3 ground = glm::vec3(cameraPosition.x + std::cos(angle) * distance, 0.0f, cameraPosition.z + std::sin(angle) * distance);
        glm::vec3 top = ground + glm::vec3(randomRange(-120.0f, 120.0f), BOLT_TOP_HEIGHT, randomRange(-120.0f, 120.0f));

        segments.clear();
        addBolt(top, ground, BOLT_WIDTH, BOLT_DEPTH, true);

        direction = glm::normalize((top + ground) * 0.5f - glm::vec3(cameraPosition.x, 0.0f, cameraPosition.z));

        pulses.clear();
        pulses.push_back({0.0f, 1.0f});
        pulses.push_back({randomRange(0.08f, 0.15f), 0.45f});
        pulses.push_back({randomRange(0.2f, 0.35f), 0.8f});

        sinceStrike = 0.0f;
        untilNextStrike = randomRange(STRIKE_INTERVAL_MIN, STRIKE_INTERVAL_MAX);
    }

    void Lightning::update(float deltaTime, float stormFactor, glm::vec3 cameraPosition, glm::vec3 viewDirection)
    {
        sinceStrike += deltaTime;

        if(stormFactor > 0.6f)
        {
            untilNextStrike -= deltaTime;
            if(untilNextStrike <= 0.0f)
                strike(cameraPosition, viewDirection);
        }

        flash = 0.0f;
        for(FlashPulse& pulse : pulses)
        {
            if(sinceStrike >= pulse.start)
                flash += pulse.strength * std::exp(-(sinceStrike - pulse.start) * FLASH_DECAY);
        }
        flash = glm::min(flash, 1.0f);
    }

    void Lightning::draw(gps::Shader shader, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition)
    {
        if(sinceStrike > BOLT_VISIBLE_TIME || segments.empty())
            return;

        std::vector<BoltVertex> vertices;
        for(BoltSegment& segment : segments)
        {
            glm::vec3 along = segment.end - segment.start;
            glm::vec3 side = glm::normalize(glm::cross(along, segment.start - cameraPosition)) * segment.width * 0.5f;
            vertices.push_back({segment.start - side, -1.0f});
            vertices.push_back({segment.start + side, 1.0f});
            vertices.push_back({segment.end - side, -1.0f});
            vertices.push_back({segment.start + side, 1.0f});
            vertices.push_back({segment.end + side, 1.0f});
            vertices.push_back({segment.end - side, -1.0f});
        }

        shader.useShaderProgram();
        GLuint program = shader.shaderProgram;
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1f(glGetUniformLocation(program, "boltBrightness"), glm::max(flash, 0.25f));

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(BoltVertex), vertices.data(), GL_DYNAMIC_DRAW);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glDepthMask(GL_FALSE);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        glBindVertexArray(0);
    }

    float Lightning::getFlash()
    {
        return flash;
    }

    glm::vec3 Lightning::getDirection()
    {
        return direction;
    }

    Lightning::~Lightning()
    {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }
}
