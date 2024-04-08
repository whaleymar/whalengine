#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Image::Image(const char* path) : mPath(path) {
    mData = stbi_load(path, &mWidth, &mHeight, &mNChannels, 0);
}

Image::~Image() {
    stbi_image_free(mData);
}
