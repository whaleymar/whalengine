#pragma once

#include "Gfx/Texture.h"
#include "Gfx/VertexObject.h"
#include "Util/Vector.h"

namespace whal {

class Texture;

// hard coded as rectangles until I need something else
struct Sprite {
    Sprite(Depth depth_ = Depth::Player, Frame frame = {}, RGB rgb = Color::WHITE);

    u32 nVertices = N_VERTS_RECT;
    Vao vao;
    Vbo vbo;
    Depth depth;
    Vector2i atlasPositionTexels;
    Vector2f scale = {1, 1};
    Vector2i frameSizeTexels = {8, 8};
    RGB color;
    bool isVertsUpdateNeeded = false;  // anim, size, color, and/or scale changed

    VertArrayRectRGBUV getVertices() const { return mVertices; }
    void updateVertices(bool flipX = false);
    void setFrameSize(s32 x, s32 y);
    void setColor(RGB rgb);

private:
    VertArrayRectRGBUV mVertices;
};

// TODO take frameSizeTexels in constructor
struct Draw {
    Draw(Depth depth_ = Depth::Player, RGB color_ = Color::WHITE);
    u32 nVertices = N_VERTS_RECT;
    Vao vao;
    Vbo vbo;
    Depth depth;
    RGB color;
    Vector2i frameSizeTexels = {8, 8};

    VertArrayRectRGB getVertices() const { return mVertices; }
    void updateVertices();
    void setFrameSize(s32, s32);

private:
    VertArrayRectRGB mVertices;
};

}  // namespace whal
