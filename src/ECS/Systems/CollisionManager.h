#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

class ActorCollider;
class SolidCollider;

class ActorsManager : public ecs::ISystem<ActorCollider> {
public:
    static std::shared_ptr<ActorsManager> getInstance() {
        static std::shared_ptr<ActorsManager> instance = ecs::ECS::getInstance().registerSystem<ActorsManager>();
        return instance;
    }

    void update() override;
    void onAdd(ecs::Entity entity) override;
    void onRemove(ecs::Entity entity) override;

    const std::vector<ActorCollider*>& getAllActors() const { return mActors; }

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
    void setUpdateNeeded() { mIsUpdateNeeded = true; }

    const std::vector<SolidCollider*>& getAllSolids() const { return mSolids; };

private:
    std::vector<SolidCollider*> mSolids;
    size_t mNumCallbackColliders = 0;
    bool mIsUpdateNeeded = false;
};

#ifndef NDEBUG
void drawColliders();
#endif

}  // namespace whal
