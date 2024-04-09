#pragma once

#include <optional>

#include "Gfx/IUseOpenGLHandle.h"
#include "Util/Expected.h"
#include "Util/Image.h"
#include "Util/Vector.h"

namespace whal {

class TextureAtlas {
public:
    TextureAtlas(const char* filename, Image& img);
    Vector2f getSize() const;

private:
    Vector2f mSize;
    bool mIsTrimEnabled;
    bool mIsRotateEnabled;
};

class Texture : public IUseOpenGLHandle {
public:
    Texture();

    std::optional<Error> loadAtlas(const char* texturePath);
    Vector2f getSize() const;
    void bind() const;
    bool isValid() const;

private:
    int mTextureID;
    std::optional<TextureAtlas> mAtlas;
    bool mIsValid = false;
};

}  // namespace whal
