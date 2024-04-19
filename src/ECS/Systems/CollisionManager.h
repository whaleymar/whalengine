#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct RigidBody;
struct SolidBody;
class ActorCollider;
class SolidCollider;

class RigidBodyManager : public ecs::ISystem<RigidBody> {
public:
    void update() override;
    void onAdd(ecs::Entity entity) override;
    void onRemove(ecs::Entity entity) override;

    const std::vector<ActorCollider*>& getAllActors() const;

private:
    std::vector<ActorCollider*> mActors;
    bool mIsUpdateNeeded = false;
};

class SolidBodyManager : public ecs::ISystem<SolidBody> {
public:
    void update() override;
    void onAdd(ecs::Entity entity) override;
    void onRemove(ecs::Entity entity) override;

    const std::vector<SolidCollider*>& getAllSolids() const;

private:
    std::vector<SolidCollider*> mSolids;
    bool mIsUpdateNeeded = false;
};

}  // namespace whal
