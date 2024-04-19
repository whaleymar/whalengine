#pragma once

// #include <vector>

#include "ECS/Systems/CollisionManager.h"

namespace whal {

class ActorCollider;
class SolidCollider;

class CollisionManager {
public:
    static CollisionManager& getInstance() {
        static CollisionManager instance;
        return instance;
    }

    void init(std::shared_ptr<RigidBodyManager> actorMgr, std::shared_ptr<SolidBodyManager> solidMgr);

    const std::vector<ActorCollider*>& getAllActors() const {
        assert(mIsValid && "CollisionManager was not initialized");
        return mActorMgr->getAllActors();
    }

    const std::vector<SolidCollider*>& getAllSolids() const {
        assert(mIsValid && "CollisionManager was not initialized");
        return mSolidMgr->getAllSolids();
    }

private:
    CollisionManager() = default;

    CollisionManager(const CollisionManager&) = delete;
    void operator=(const CollisionManager&) = delete;

    std::shared_ptr<RigidBodyManager> mActorMgr;
    std::shared_ptr<SolidBodyManager> mSolidMgr;

#ifndef NDEBUG
    bool mIsValid = false;
#endif
};

#ifndef NDEBUG
void drawColliders();
#endif

}  // namespace whal
