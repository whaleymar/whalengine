#pragma once

#include <vector>

#include "Physics/IUseCollision.h"
#include "Physics/Material.h"
#include "Util/Vector.h"

namespace whal {

class ActorCollider;
struct HitInfo;
namespace ecs {
class Entity;
}

// using TriggerCallback = void (*)(HitInfo, Entity self, Entity other);
using ActorCollisionCallback = void (*)(ActorCollider* selfCollider, ecs::Entity actorEntity, HitInfo hitinfo);
// using CollisionCallback = void (*)(ecs::Entity self, HitInfo hitinfo);

class SolidCollider : public IUseCollision {
public:
    SolidCollider() = default;
    SolidCollider(Vector2f position, Vector2i half, Material material = Material::None, ActorCollisionCallback onCollisionEnter_ = nullptr,
                  s32 xCollisionNormal = 0, s32 yCollisionNormal = 0);

    void move(f32 x, f32 y, bool isManualMove = false);
    std::vector<ActorCollider*> getRidingActors() const;
    void setCollisionCallback(ActorCollisionCallback callback);
    ActorCollisionCallback getOnCollisionEnter() const { return mOnCollisionEnter; }
    Vector2i getCollisionNormal() const { return mCollisionNormal; }
    void setCollisionNormal(Vector2i normal) { mCollisionNormal = normal; }

private:
    void moveDirection(f32 toMoveRounded, f32 toMoveUnrounded, bool isXDirection, f32 solidEdge, EdgeGetter edgeFunc,
                       std::vector<ActorCollider*>& riding, bool isManualMove);
    Vector2i mCollisionNormal;  // 0 for either direction, 1 for top/right, -1 for bottom/left
    ActorCollisionCallback mOnCollisionEnter;
};

class ActorCollider : public IUseCollision {
public:
    ActorCollider() = default;
    ActorCollider(Vector2f position, Vector2i half);

    std::optional<HitInfo> moveX(const Vector2f amount, const ActorCollisionCallback callback);
    std::optional<HitInfo> moveY(const Vector2f amount, const ActorCollisionCallback callback);
    bool isAlive() const { return mIsAlive; }
    void setMomentum(const f32 momentum, const bool isXDirection);
    void addMomentum(const f32 momentum, const bool isXDirection);
    void maintainMomentum(const bool isXDirection);
    void resetMomentumX();
    void resetMomentumY();
    void resetMomentum();
    Vector2f getMomentum() const { return mStoredMomentum; }
    bool isMomentumStoredX() const { return mMomentumFramesLeft.x() > 0; }
    bool isMomentumStoredY() const { return mMomentumFramesLeft.y() > 0; }
    void momentumNotUsed();
    bool checkIsGrounded(const std::vector<std::pair<ecs::Entity, SolidCollider*>>& solids, SolidCollider** groundSolid);

    template <typename T>
    std::optional<HitInfo> checkCollision(const std::vector<std::pair<ecs::Entity, T*>>& objects, const Vector2i position,
                                          const Vector2i moveNormal) const;

    // squish is a CollisionCallback. Not sure if I will define others
    // TODO these can't be virtual bc ECS
    virtual void squish(const HitInfo hitInfo);
    virtual bool isRiding(const SolidCollider* solid) const;

private:
    bool tryCornerCorrection(const std::vector<std::pair<ecs::Entity, SolidCollider*>>& solids, Vector2i nextPos, s32 moveSign);

    Vector2f mStoredMomentum;
    Vector2i mMomentumFramesLeft = {0, 0};
    // f32 mMass = 1; // could give solids a mass and use mass ratio to calculate force
    bool mIsAlive = true;
};

void defaultSquish(ActorCollider* selfCollider, ecs::Entity self, HitInfo hitinfo);

}  // namespace whal
