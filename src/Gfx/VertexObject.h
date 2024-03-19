#pragma once

#include <array>

#include "Gfx/IUseOpenGLHandle.h"
#include "Util/Types.h"

namespace whal {

class Vao : IUseOpenGLHandle {
public:
    Vao();
    ~Vao();

    void bind() const;
};

class Vbo : IUseOpenGLHandle {
public:
    Vbo();
    ~Vbo();

    void bind() const;
    void buffer(f32* vertices, u32 sizeBytes) const;
};

std::array<f32, 30> MakeRectVertices(f32 pixelWidth, f32 pixelHeight);

}  // namespace whal
