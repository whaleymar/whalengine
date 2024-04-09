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

Sprite::Sprite(Depth depth_, Vector2i atlasPositionTexels_, RGB rgb) : depth(depth_), atlasPositionTexels(atlasPositionTexels_), color(rgb) {
    updateVertices();
}

void Sprite::setFrameSize(s32 frameSizeX, s32 frameSizeY) {
    frameSizeTexels = {frameSizeX, frameSizeY};
    isVertsUpdateNeeded = true;
}

void Sprite::setColor(RGB rgb) {
    color = rgb;
    isVertsUpdateNeeded = true;
}

void Sprite::updateVertices() {
    const f32 width = getPixelSize(frameSizeTexels.x(), scale.x());
    const f32 height = getPixelSize(frameSizeTexels.y(), scale.y());

    Vector2f sheetSize = GLResourceManager::getInstance().getTexture(TEXNAME_SPRITE).getSize();
    f32 scaleX = 1 / sheetSize.x();
    f32 scaleY = 1 / sheetSize.y();

    Vector2f texelOffset = toFloatVec(atlasPositionTexels);
    f32 xMin = texelOffset.x() * scaleX;
    f32 xMax = (texelOffset.x() + frameSizeTexels.x()) * scaleX;
    f32 yMin = texelOffset.y() * scaleY;
    f32 yMax = (texelOffset.y() + frameSizeTexels.y()) * scaleY;

    mVertices = MakeRectVerticesRGBUV(width, height, depth, color, xMin, xMax, yMax, yMin);
    isVertsUpdateNeeded = false;
}

Draw::Draw(Depth depth_, RGB color_) : depth(depth_), color(color_) {
    updateVertices();
}

void Draw::setFrameSize(s32 frameSizeX, s32 frameSizeY) {
    frameSizeTexels = {frameSizeX, frameSizeY};
    updateVertices();
}

void Draw::updateVertices() {
    const f32 width = getPixelSize(frameSizeTexels.x(), 1.0);
    const f32 height = getPixelSize(frameSizeTexels.y(), 1.0);

    mVertices = MakeRectVerticesRGB(width, height, depth, color);
}

}  // namespace whal
