#include "Position.h"
#include "Gfx/GfxUtil.h"

namespace whal {

Position Position::texels(s32 x, s32 y) {
    static const s32 PPTexel = static_cast<s32>(PIXELS_PER_TEXEL);
    return Position({x * PPTexel, y * PPTexel});
}

Position Position::tiles(s32 x, s32 y) {
    static const s32 PPTile = static_cast<s32>(PIXELS_PER_TEXEL * TEXELS_PER_TILE);
    return Position({x * PPTile, y * PPTile});
}

}  // namespace whal
