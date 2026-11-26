#include "graphics/ViewMode.hpp"

namespace gps {

    void ViewMode::setPolygonMode(PolygonMode mode)
    {
        polygonMode = mode;
    }

    void ViewMode::toggleFlatShading()
    {
        flatShading = !flatShading;
    }

    void ViewMode::apply()
    {
        switch(polygonMode)
        {
        case PolygonMode::Solid:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case PolygonMode::Wireframe:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case PolygonMode::Points:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glPointSize(2.0f);
            break;
        }
    }

    bool ViewMode::isFlatShading()
    {
        return flatShading;
    }

    std::string ViewMode::describe()
    {
        std::string text;
        switch(polygonMode)
        {
        case PolygonMode::Solid:
            text = "solid";
            break;
        case PolygonMode::Wireframe:
            text = "wireframe";
            break;
        case PolygonMode::Points:
            text = "points";
            break;
        }

        if(flatShading)
            text += ", flat";
        else
            text += ", smooth";

        return text;
    }
}
