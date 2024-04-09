#pragma once

#include <array>

#include "Gfx/Color.h"
#include "Gfx/IUseOpenGLHandle.h"
#include "Util/Types.h"

namespace whal {

static constexpr u32 STRIDE_SIZE_RGBUV = 8;
static constexpr u32 STRIDE_SIZE_RGB = 6;
static constexpr u32 STRIDE_SIZE_UV = 5;
static constexpr u32 STRIDE_SIZE_XYZ = 3;
static constexpr u32 N_VERTS_RECT = 4;

using VertArrayRectRGBUV = std::array<f32, STRIDE_SIZE_RGBUV * N_VERTS_RECT>;
using VertArrayRectRGB = std::array<f32, STRIDE_SIZE_RGB * N_VERTS_RECT>;
using VertArrayRectUV = std::array<f32, STRIDE_SIZE_UV * N_VERTS_RECT>;
using VertArrayRect = std::array<f32, STRIDE_SIZE_XYZ * N_VERTS_RECT>;

class Vao : IUseOpenGLHandle {
public:
    Vao();

    void bind() const;
    void free();
};

class Vbo : IUseOpenGLHandle {
public:
    Vbo();

    void bind() const;
    void buffer(f32* vertices, u32 sizeBytes) const;
    void free();
};

enum class Depth {
    BackgroundFar,   // parallax
    BackgroundMid,   // parallax
    BackgroundNear,  // parallax
    BackgroundNoParallax,
    Player,
    Foreground,
    Debug
};

constexpr f32 depthToFloat(Depth depth);

VertArrayRect MakeRectVertices(f32 pixelWidth, f32 pixelHeight, Depth depth);
VertArrayRectUV MakeRectVerticesUV(f32 pixelWidth, f32 pixelHeight, Depth depth, f32 minU = 0.0, f32 maxU = 1.0, f32 minV = 0.0, f32 maxV = 1.0);
VertArrayRectRGB MakeRectVerticesRGB(f32 pixelWidth, f32 pixelHeight, Depth depth, const RGB rgb = Color::WHITE);
VertArrayRectRGBUV MakeRectVerticesRGBUV(f32 pixelWidth, f32 pixelHeight, Depth depth, const RGB rgb = Color::WHITE, f32 minU = 0.0, f32 maxU = 1.0,
                                         f32 minV = 0.0, f32 maxV = 1.0);

}  // namespace whal
