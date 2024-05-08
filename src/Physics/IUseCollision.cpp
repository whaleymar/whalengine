#include "IUseCollision.h"

namespace whal {

bool checkDirectionalCollision(const AABB& actor, const AABB& solid, Vector2i moveNormal, CollisionDir collisionDir) {
    switch (collisionDir) {
    case CollisionDir::ALL:
        return true;

    case CollisionDir::LEFT:
        if (moveNormal.x() <= 0 || actor.right() != solid.left()) {
            return false;
        }
        break;

    case CollisionDir::RIGHT:
        if (moveNormal.x() >= 0 || actor.left() != solid.right()) {
            return false;
        }
        break;

    case CollisionDir::DOWN:
        if (moveNormal.y() <= 0 || actor.top() != solid.bottom()) {
            return false;
        }
        break;

    case CollisionDir::UP:
        if (moveNormal.y() >= 0 || actor.bottom() != solid.top()) {
            return false;
        }
        break;
    }

    return true;
}

}  // namespace whal
