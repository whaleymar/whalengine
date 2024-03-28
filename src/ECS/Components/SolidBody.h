#pragma once

#include "Gfx/GfxUtil.h"
#include "Physics/Solid.h"

namespace whal {

struct SolidBody {
    SolidBody(s32 half_ = TILE_LEN_PIXELS);
    SolidBody(Vector2f center, s32 halfx, s32 halfy);

    SolidCollider collider;
};

}  // namespace whal
