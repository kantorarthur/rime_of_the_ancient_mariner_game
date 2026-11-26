#ifndef ViewMode_hpp
#define ViewMode_hpp

#include "core/OpenGL.hpp"

#include <string>

namespace gps {

    enum class PolygonMode {Solid, Wireframe, Points};

    class ViewMode
    {
    public:
        void setPolygonMode(PolygonMode mode);
        void toggleFlatShading();

        void apply();
        bool isFlatShading();
        std::string describe();

    private:
        PolygonMode polygonMode = PolygonMode::Solid;
        bool flatShading = false;
    };
}

#endif
