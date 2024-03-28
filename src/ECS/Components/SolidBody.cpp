#include "SolidBody.h"

#include "Physics/CollisionManager.h"
#include "Util/Vector.h"

namespace whal {

SolidBody::SolidBody(s32 half_) : collider(Vector2f(), Vector2i(half_, half_)) {}

SolidBody::SolidBody(Vector2f center, s32 halfx, s32 halfy) : collider(center, Vector2i(halfx, halfy)) {}

}  // namespace whal
