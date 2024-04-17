#include "Physics/Collision/HitInfo.h"
#include "Util/Vector.h"

namespace whal {

HitInfo::HitInfo() {
    mPos = Vector2i::zero;
    mDelta = Vector2i::zero;
    mNormal = Vector2i::zero;
}

HitInfo::HitInfo(Vector2i position, Vector2i delta, Vector2i normal) : mPos(position), mDelta(delta), mNormal(normal){};
}  // namespace whal
