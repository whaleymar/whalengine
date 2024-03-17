#include "Physics/Collision/HitInfo.h"
#include "Util/Vector.h"

namespace whal {

HitInfo::HitInfo() {
    mPos = Vector::zero2i;
    mDelta = Vector::zero2i;
    mNormal = Vector::zero2i;
}

HitInfo::HitInfo(Vector2i position, Vector2i delta, Vector2i normal) : mPos(position), mDelta(delta), mNormal(normal){};
}  // namespace whal
