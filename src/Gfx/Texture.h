#pragma once

#include <memory>
#include <optional>

#include "Gfx/IUseOpenGLHandle.h"
#include "Util/Expected.h"
#include "Util/Image.h"
#include "Util/Vector.h"

namespace whal {

class TextureAtlas {
public:
    TextureAtlas(const char* filename);

    Vector2i getSize() const;
    const u8* getImageData() const;

private:
    std::unique_ptr<Image> mImage;
    bool mIsTrimEnabled;
    bool mIsRotateEnabled;
};

class Texture : public IUseOpenGLHandle {
public:
    Texture();
    // ~Texture(); // TODO free handles?

    std::optional<Error> loadAtlas(const char* texturePath);
    Vector2i getSize() const;
    void bind() const;
    bool isValid() const;

private:
    int mTextureID;
    std::optional<TextureAtlas> mAtlas;
    bool mIsValid = false;
};

}  // namespace whal
