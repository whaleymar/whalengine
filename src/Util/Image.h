#pragma once

#include "Util/Types.h"

class Image {
public:
    Image(const char* path);
    ~Image();

    s32 width() const { return mWidth; }
    s32 height() const { return mHeight; }
    const u8* getData() const { return mData; }

private:
    u8* mData;
    const char* mPath;
    s32 mWidth;
    s32 mHeight;
    s32 mNChannels;
};
