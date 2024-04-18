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
    RGB color;
    bool isVertsUpdateNeeded = false;  // anim, size, color, and/or scale changed

    VertArrayRectRGBUV getVertices() const { return mVertices; }
    void updateVertices(bool flipX = false);
    void setFrameSize(s32 x, s32 y);
    Vector2i getFrameSizeTexels() const { return mFrameSizeTexels; }
    void setColor(RGB rgb);

private:
    VertArrayRectRGBUV mVertices;
    Vector2i mFrameSizeTexels;
};

struct Draw {
    Draw(RGB color_ = Color::WHITE, Vector2i frameSizeTexels_ = {8, 8}, Depth depth_ = Depth::Player);
    u32 nVertices = N_VERTS_RECT;
    Vao vao;
    Vbo vbo;
    Depth depth;
    RGB color;
    Vector2i frameSizeTexels;

    VertArrayRectRGB getVertices() const { return mVertices; }
    void updateVertices();
    void setFrameSize(s32, s32);

private:
    VertArrayRectRGB mVertices;
};

}  // namespace whal
