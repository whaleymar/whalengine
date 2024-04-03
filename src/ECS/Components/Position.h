#pragma once

#include "Util/Vector.h"

namespace whal {

// entity position in pixels
struct Position {
    Vector2i e;

    static Position texels(s32 x, s32 y);
    static Position tiles(s32 x, s32 y);
};

}  // namespace whal
