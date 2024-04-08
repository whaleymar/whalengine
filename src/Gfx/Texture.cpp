#include "Gfx/Texture.h"

#include <GL/gl.h>
#include <format>

#include "Util/FileIO.h"

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

    Image img(texturePath);
    mAtlas = TextureAtlas(texturePath, img);

    bind();

    // GL_NEAREST for pixelated look (no interp)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // repeat at end of texture https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexParameter.xhtml
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getData());

    mIsValid = true;

    return std::nullopt;
}

Vector2f Texture::getSize() const {
    return mAtlas->getSize();
}

void Texture::bind() const {
    glActiveTexture(mTextureID);
    glBindTexture(GL_TEXTURE_2D, mHandle);
}
bool Texture::isValid() const {
    return mAtlas != std::nullopt;
}

TextureAtlas::TextureAtlas(const char* filename, Image& img) {
    mSize = {static_cast<f32>(img.width()), static_cast<f32>(img.height())};
    // TODO
    mIsTrimEnabled = false;
    mIsRotateEnabled = false;
}

Vector2f TextureAtlas::getSize() const {
    return mSize;
}

}  // namespace whal
