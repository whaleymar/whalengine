#pragma once

#include "Util/Vector.h"

namespace whal {

enum Facing : u8 {
    Left = 0,
    Right = 1,
};

// entity position in pixels
struct Position {
    Vector2i e;
    Facing facing = Facing::Left;

    static Position texels(s32 x, s32 y);
    static Position tiles(s32 x, s32 y);
};

}  // namespace whal
