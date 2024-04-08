#pragma once

#include <array>

#include "Gfx/IUseOpenGLHandle.h"
#include "Util/Types.h"
#include "Util/Vector.h"

namespace whal {

static constexpr u32 STRIDE_SIZE = 5;
static constexpr u32 STRIDE_SIZE_XYZ = 3;
static constexpr u32 N_VERTS_RECT = 4;

using VertArrayRect = std::array<f32, STRIDE_SIZE * N_VERTS_RECT>;
using VertArrayLine = std::array<f32, STRIDE_SIZE_XYZ * N_VERTS_RECT>;

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

f32 depthToFloat(Depth depth);

VertArrayRect MakeRectVertices(f32 pixelWidth, f32 pixelHeight, f32 minU = 0.0, f32 maxU = 1.0, f32 minV = 0.0, f32 maxV = 1.0);

}  // namespace whal
