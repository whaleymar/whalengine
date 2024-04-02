#pragma once

#include <array>

#include "Gfx/VertexObject.h"
#include "Util/Vector.h"

namespace whal {

struct Draw {
    Draw(s32 depth_ = 0);

    u32 nVertices = 6;
    Vao vao;
    Vbo vbo;
    s32 depth;
    Vector2f scale = {1, 1};
    Vector2i atlasPositionTexels = {0, 0};
    Vector2i frameSizeTexels = {8, 8};
    bool isUVUpdateNeeded = false;

    std::array<float, 30> getVertices() const { return mVertices; }
    void updateVertices();

private:
    std::array<float, 30> mVertices;  // will hard code until i need something different
};

}  // namespace whal
