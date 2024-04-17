#include "GfxUtil.h"

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

}  // namespace whal
