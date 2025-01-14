#pragma once

#include <optional>
#include <unordered_map>

#include "Gfx/IUseOpenGLHandle.h"
#include "Util/Expected.h"
#include "Util/Image.h"
#include "Util/Vector.h"

namespace whal {

struct Frame {
    Vector2i atlasPositionTexels;
    Vector2i dimensionsTexels;
};

class TextureAtlas {
public:
    TextureAtlas(Image& img);
    std::optional<Error> init();
    Vector2f getSize() const;
    std::optional<Frame> getFrame(const char* name) const;
    bool isValid() const { return mIsValid; }

private:
    Vector2f mSize;
    std::unordered_map<std::string, Frame> mTable;
    bool mIsTrimEnabled = false;
    bool mIsRotateEnabled = false;
    bool mIsValid = false;
};

class Texture : public IUseOpenGLHandle {
public:
    Texture();

    std::optional<Error> loadAtlas(const char* texturePath);
    Vector2f getSize() const;
    void bind() const;
    bool isValid() const;
    std::optional<Frame> getFrame(const char* name) const;

private:
    int mTextureID;
    std::optional<TextureAtlas> mAtlas;
    bool mIsValid = false;
};

}  // namespace whal
