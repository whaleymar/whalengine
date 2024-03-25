#include "Gfx/Texture.h"

#include <GL/gl.h>
#include <format>

// #include "CImg.h"

#include "Util/FileIO.h"

namespace whal {

Texture::Texture() {
    glGenTextures(1, &mHandle);
}

Texture::~Texture() {
    if (mData) {
        free(mData);
    }
}

std::optional<Error> Texture::loadAtlas(const char* texturePath) {
    // using namespace cimg_library;
    // if (!texturePath) {
    //     return Error("Cannot load empty file name");
    // }
    // if (!isExist(texturePath)) {
    //     return Error(std::format("%s does not exist", texturePath));
    // }
    // CImg<u8> image;
    // image.load_png(texturePath);
    //
    // mSize = Vector2i(image.width(), image.height());
    // mData = new f32[image.width() * image.height() * image.spectrum()];
    //
    // cimg_forXYC(image, x, y, c) {
    //     mData[(y * image.width() + x) * image.spectrum() + c] = static_cast<f32>(image(x, y, c)) / 255.0f;
    // }

    bind();

    // GL_NEAREST for pixelated look (no interp)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // repeat at end of texture https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexParameter.xhtml
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mSize.x(), mSize.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (u8*)mData);

    return std::nullopt;
}

Vector2i Texture::getSize() const {
    return mSize;
}

void Texture::bind() const {
    glActiveTexture(GL_TEXTURE0);  // TODO member variable
    glBindTexture(GL_TEXTURE_2D, mHandle);
}

}  // namespace whal
