#pragma once

#include <vector>

#include "Physics/Collision/HitInfo.h"
#include "Physics/IUseCollision.h"
#include "Util/Types.h"
#include "Util/Vector.h"

namespace whal {

class SolidCollider;
class ActorCollider;

using CollisionCallback = void (ActorCollider::*)(HitInfo);

class ActorCollider : public IUseCollision {
public:
    ActorCollider(Vector2f position, Vector2i half);

    void moveDirection(const bool isXDirection, const f32 amount, const CollisionCallback callback);
    bool isAlive() const { return mIsAlive; }

    template <typename T>
    std::optional<HitInfo> checkCollision(const std::vector<T*>& objects, const Vector2i position) const;

    // squish is a CollisionCallback. Not sure if I will define others
    virtual void squish(const HitInfo hitInfo);
    virtual bool isRiding(const SolidCollider* solid) const;

private:
    bool mIsAlive = true;
};

}  // namespace whal
