#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct RigidBody;
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

class SolidsManager : public ecs::ISystem<SolidCollider> {
public:
    static std::shared_ptr<SolidsManager> getInstance() {
        static std::shared_ptr<SolidsManager> instance = ecs::ECS::getInstance().registerSystem<SolidsManager>();
        return instance;
    }
    void update() override;
    void onAdd(ecs::Entity entity) override;
    void onRemove(ecs::Entity entity) override;

    const std::vector<SolidCollider*>& getAllSolids() const { return mSolids; };

private:
    std::vector<SolidCollider*> mSolids;
    bool mIsUpdateNeeded = false;
};

}  // namespace whal
