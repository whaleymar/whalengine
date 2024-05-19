#include "GfxUtil.h"
#include "ECS/Systems/TagTrackers.h"

namespace whal {

const char* toString(ShaderType shaderType) {
    switch (shaderType) {
    case ShaderType::SpritePlain:
        return "sprite";
    case ShaderType::SpriteRGB:
        return "sprite_rgb";
    case ShaderType::Debug:
        return "debug";
    case ShaderType::RGBonly:
        return "color";
    }
}

Vector2i screenToWorldCoords(Vector2i screenCoords) {
    auto cameraPos = getCameraPosition();

    f32 multX = static_cast<f32>(WINDOW_WIDTH_PIXELS) / static_cast<f32>(WINDOW_WIDTH_PIXELS_ACTUAL);
    f32 multY = static_cast<f32>(WINDOW_HEIGHT_PIXELS) / static_cast<f32>(WINDOW_HEIGHT_PIXELS_ACTUAL);
    auto middleOffset = Vector2i(WINDOW_WIDTH_PIXELS / 2, WINDOW_HEIGHT_PIXELS / 2);

    Vector2i yAtTop = Vector2i(screenCoords.x() * multX, (static_cast<s32>(WINDOW_HEIGHT_PIXELS_ACTUAL) - screenCoords.y()) * multY);
    return yAtTop + cameraPos - middleOffset;
}

}  // namespace whal
