#include "ECS/Components/Draw.h"

#include "Gfx/GLResourceManager.h"
#include "Gfx/GfxUtil.h"
#include "Gfx/Texture.h"
#include "Gfx/VertexObject.h"
#include "Util/Vector.h"

namespace whal {

constexpr f32 getPixelSize(const s32 frameSize, const f32 scale) {
    return static_cast<f32>(frameSize) * PIXELS_PER_TEXEL * scale;
}

Sprite::Sprite(s32 depth_, Vector2i atlasPositionTexels_) : depth(depth_), atlasPositionTexels(atlasPositionTexels_) {
    updateVertices();
}

void Sprite::setFrameSize(s32 frameSizeX, s32 frameSizeY) {
    frameSizeTexels = {frameSizeX, frameSizeY};
    updateVertices();
}

void Sprite::updateVertices() {
    const f32 width = getPixelSize(frameSizeTexels.x(), scale.x());
    const f32 height = getPixelSize(frameSizeTexels.y(), scale.y());

    Vector2f sheetSize = toFloatVec(GLResourceManager::getInstance().getTexture(TEXNAME_SPRITE).getSize());
    f32 scaleX = 1 / sheetSize.x();
    f32 scaleY = 1 / sheetSize.y();

    Vector2f pixelOffset = toFloatVec(atlasPositionTexels) * PIXELS_PER_TEXEL;
    f32 xMin = pixelOffset.x() * scaleX;
    f32 xMax = (pixelOffset.x() + frameSizeTexels.x() * PIXELS_PER_TEXEL) * scaleX;
    f32 yMin = pixelOffset.y() * scaleY;
    f32 yMax = (pixelOffset.y() + frameSizeTexels.y() * PIXELS_PER_TEXEL) * scaleY;

    mVertices = MakeRectVertices(width, height, xMin, xMax, yMin, yMax);
}

Draw::Draw(s32 depth_) : depth(depth_) {
    updateVertices();
}

void Draw::setFrameSize(s32 frameSizeX, s32 frameSizeY) {
    frameSizeTexels = {frameSizeX, frameSizeY};
    updateVertices();
}

void Draw::updateVertices() {
    const f32 width = getPixelSize(frameSizeTexels.x(), 1.0);
    const f32 height = getPixelSize(frameSizeTexels.y(), 1.0);

    mVertices = MakeRectVertices(width, height);
}

}  // namespace whal
