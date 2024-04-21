#pragma once

#include <vector>

#include "Physics/IUseCollision.h"
#include "Util/Vector.h"

namespace whal {

struct TriggerCollider {};

class ActorCollider;
struct HitInfo;

class SolidCollider : public IUseCollision {
public:
    SolidCollider() = default;
    SolidCollider(Vector2f position, Vector2i half);

    void move(f32 x, f32 y, bool isManualMove = false);
    std::vector<ActorCollider*> getRidingActors() const;

private:
    void moveDirection(f32 toMove, bool isXDirection, f32 solidEdge, EdgeGetter edgeFunc, std::vector<ActorCollider*>& riding, bool isManualMove);
};

using CollisionCallback = void (ActorCollider::*)(HitInfo);

class ActorCollider : public IUseCollision {
public:
    ActorCollider() = default;
    ActorCollider(Vector2f position, Vector2i half);

    bool moveX(const Vector2f amount, const CollisionCallback callback);
    bool moveY(const Vector2f amount, const CollisionCallback callback);
    bool isAlive() const { return mIsAlive; }
    void setMomentum(const f32 momentum, const bool isXDirection);
    void addMomentum(const f32 momentum, const bool isXDirection);
    void resetMomentum();
    Vector2f getMomentum() const { return mStoredMomentum; }
    bool isMomentumStored() const { return mMomentumFramesLeft > 0; }
    void momentumNotUsed();
    bool checkIsGrounded(const std::vector<SolidCollider*>& solids);

    template <typename T>
    std::optional<HitInfo> checkCollision(const std::vector<T*>& objects, const Vector2i position) const;

    // squish is a CollisionCallback. Not sure if I will define others
    virtual void squish(const HitInfo hitInfo);
    virtual bool isRiding(const SolidCollider* solid) const;

private:
    bool tryCornerCorrection(const std::vector<SolidCollider*>& solids, Vector2i nextPos, s32 moveSign);

    Vector2f mStoredMomentum;
    s32 mMomentumFramesLeft = 0;
    // f32 mMass = 1; // could give solids a mass and use mass ratio to calculate force
    bool mIsAlive = true;
};

}  // namespace whal
