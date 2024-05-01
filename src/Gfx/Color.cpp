#include "Color.h"

namespace whal {

RGB RGB::fromInts(s32 r, s32 g, s32 b) {
    return {static_cast<f32>(r) / 255.0f, static_cast<f32>(g) / 255.0f, static_cast<f32>(b) / 255.0f};
}

}  // namespace whal
