#include "Color.h"

#include <sstream>

namespace whal {

RGB RGB::fromInts(s32 r, s32 g, s32 b) {
    return {static_cast<f32>(r) / 255.0f, static_cast<f32>(g) / 255.0f, static_cast<f32>(b) / 255.0f};
}

RGB RGB::fromHexStringARGB(std::string hexString) {
    s32 r, g, b;
    hexString = hexString.erase(0, 3);  // remove "#" and alpha
    std::istringstream(hexString.substr(0, 2)) >> std::hex >> r;
    std::istringstream(hexString.substr(2, 2)) >> std::hex >> g;
    std::istringstream(hexString.substr(4, 2)) >> std::hex >> b;

    return RGB::fromInts(r, g, b);
}

}  // namespace whal
