#pragma once

#include "Gfx/VertexObject.h"
#include "Util/Vector.h"

namespace whal {

// hard coded as rectangles until I need something else
struct Draw {
    Draw(s32 depth_ = 0);

    u32 nVertices = N_VERTS_RECT;
    Vao vao;
    Vbo vbo;
    s32 depth;
    Vector2f scale = {1, 1};
    Vector2i atlasPositionTexels = {0, 0};  // dont think i want sprite component, but need sprites to be optional. nullable sprite struct member?
    Vector2i frameSizeTexels = {8, 8};
    bool isVertsUpdateNeeded = false;

    VertArrayRect getVertices() const { return mVertices; }
    void updateVertices();
    void setFrameSize(s32, s32);

private:
    VertArrayRect mVertices;
};

}  // namespace whal
