#include "ECS/Components/Draw.h"

#include "Gfx/GfxUtil.h"
#include "Gfx/VertexObject.h"

namespace whal {

constexpr f32 getPixelSize(const s32 frameSize, const f32 scale) {
    return static_cast<f32>(frameSize) * PIXELS_PER_TEXEL * scale;
}

Draw::Draw(s32 depth_) : depth(depth_) {
    const f32 width = getPixelSize(frameSizeTexels.x(), scale.x());
    const f32 height = getPixelSize(frameSizeTexels.y(), scale.y());
    vertices = MakeRectVertices(width, height);
}

}  // namespace whal
