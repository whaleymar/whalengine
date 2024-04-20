#include "Physics/Collision/HitInfo.h"
#include "Util/Vector.h"

namespace whal {

HitInfo::HitInfo() {}

HitInfo::HitInfo(Vector2i normal, AABB* other_) : normal(normal), other(other_){};
}  // namespace whal
