#include "Transform.h"

#include "Gfx/GfxUtil.h"

namespace whal {

Transform Transform::texels(s32 x, s32 y) {
    static const s32 PPTexel = static_cast<s32>(PIXELS_PER_TEXEL);
    return Transform({x * PPTexel, y * PPTexel});
}

Transform Transform::tiles(s32 x, s32 y) {
    static const s32 PPTile = static_cast<s32>(PIXELS_PER_TEXEL * TEXELS_PER_TILE);
    return Transform({x * PPTile, y * PPTile});
}

}  // namespace whal
