#pragma once

#include "Util/Types.h"

namespace whal {

struct RGB {
    f32 r = 1.0;
    f32 g = 1.0;
    f32 b = 1.0;
};

namespace Color {

const RGB WHITE = {1.0, 1.0, 1.0};
const RGB RED = {1.0, 0.0, 0.0};
const RGB MAGENTA = {1.0, 0.0, 1.0};
const RGB EMERALD = {0.314, 0.8, 0.377};

}  // namespace Color

}  // namespace whal
