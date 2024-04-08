#include "Gfx/Texture.h"

#include <GL/gl.h>
#include <format>
#include <memory>

#include "Util/FileIO.h"
#include "Util/Image.h"

namespace whal {

static int TEXTURE_ID = GL_TEXTURE0;

Texture::Texture() {
    glGenTextures(1, &mHandle);
    mTextureID = TEXTURE_ID++;
}

std::optional<Error> Texture::loadAtlas(const char* texturePath) {
    if (!texturePath) {
        return Error("Cannot load empty file name");
    }
    if (!isExist(texturePath)) {
        return Error(std::format("%s does not exist", texturePath));
    }

    mAtlas = TextureAtlas(texturePath);

    bind();

    // GL_NEAREST for pixelated look (no interp)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // repeat at end of texture https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexParameter.xhtml
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    auto size = getSize();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x(), size.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, mAtlas->getImageData());

    mIsValid = true;

    return std::nullopt;
}

Vector2i Texture::getSize() const {
    return mAtlas->getSize();
}

void Texture::bind() const {
    glActiveTexture(mTextureID);
    glBindTexture(GL_TEXTURE_2D, mHandle);
}
bool Texture::isValid() const {
    return mAtlas != std::nullopt;
}

TextureAtlas::TextureAtlas(const char* filename) : mImage(std::make_unique<Image>(filename)) {
    // TODO
    mIsTrimEnabled = false;
    mIsRotateEnabled = false;
}

Vector2i TextureAtlas::getSize() const {
    return {mImage->width(), mImage->height()};
}

const u8* TextureAtlas::getImageData() const {
    return mImage->getData();
}

}  // namespace whal
