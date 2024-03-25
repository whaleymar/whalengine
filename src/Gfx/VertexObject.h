#pragma once

#include <array>

#include "Gfx/IUseOpenGLHandle.h"
#include "Util/Types.h"

namespace whal {

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

std::array<f32, 30> MakeRectVertices(f32 pixelWidth, f32 pixelHeight);

}  // namespace whal
