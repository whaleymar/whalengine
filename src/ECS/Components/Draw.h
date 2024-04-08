#pragma once

#include "Gfx/VertexObject.h"
#include "Util/Vector.h"

namespace whal {

class Texture;

// hard coded as rectangles until I need something else
struct Sprite {
    Sprite(s32 depth_ = 0, Vector2i atlasPositionTexels_ = {0, 0});

    u32 nVertices = N_VERTS_RECT;
    Vao vao;
    Vbo vbo;
    s32 depth;
    Vector2i atlasPositionTexels = {0, 0};
    Vector2f scale = {1, 1};
    Vector2i frameSizeTexels = {8, 8};
    bool isVertsUpdateNeeded = false;  // anim or scale changed

    VertArrayRect getVertices() const { return mVertices; }
    void updateVertices();
    void setFrameSize(s32, s32);

private:
    VertArrayRect mVertices;
};

struct Draw {
    Draw(s32 depth_ = 0);
    u32 nVertices = N_VERTS_RECT;
    Vao vao;
    Vbo vbo;
    s32 depth;
    Vector2i frameSizeTexels = {8, 8};

    VertArrayRect getVertices() const { return mVertices; }
    void updateVertices();
    void setFrameSize(s32, s32);

private:
    VertArrayRect mVertices;
};

}  // namespace whal
