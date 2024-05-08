#pragma once

#include <string>

#include "Util/Types.h"

namespace whal {

struct RGB {
    f32 r = 1.0;
    f32 g = 1.0;
    f32 b = 1.0;

    static RGB fromInts(s32 r, s32 g, s32 b);
    static RGB fromHexStringARGB(std::string hexString);
};

namespace Color {

static const RGB WHITE = {1.0, 1.0, 1.0};
static const RGB RED = {1.0, 0.0, 0.0};
static const RGB MAGENTA = {1.0, 0.0, 1.0};
static const RGB EMERALD = {0.314, 0.8, 0.377};
static const RGB PURPLE = RGB::fromInts(198, 51, 242);
static const RGB PINK = RGB::fromInts(242, 116, 217);

}  // namespace Color

}  // namespace whal
