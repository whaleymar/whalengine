#pragma once

#include <array>

#include "Gfx/VertexObject.h"
#include "Util/Vector.h"

namespace whal {

struct Draw {
    Draw(s32 depth_ = 0);

    std::array<float, 30> vertices;  // will hard code until i need something different
    u32 nVertices = 6;
    Vao vao;
    Vbo vbo;
    s32 depth;
    Vector2i scale = {1, 1};
    Vector2i atlasPosition = {0, 0};
    Vector2i frameSize = {8, 8};
    bool isUVUpdateNeeded = false;
};

}  // namespace whal
