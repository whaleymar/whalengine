#pragma once

#include <optional>

#include "Gfx/IUseOpenGLHandle.h"
#include "Util/Expected.h"
#include "Util/Vector.h"

namespace whal {

class Texture : public IUseOpenGLHandle {
public:
    Texture();
    ~Texture();

    std::optional<Error> loadAtlas(const char* texturePath);
    Vector2i getSize() const;
    void bind() const;

private:
    Vector2i mSize;
    f32* mData = nullptr;
};

}  // namespace whal
