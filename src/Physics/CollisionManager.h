#pragma once

#include <vector>
#include "Physics/Actor.h"

namespace whal {

class CollisionManager {
public:
    static CollisionManager& getInstance() {
        static CollisionManager instance;
        return instance;
    }

    std::vector<ActorCollider*>& getAllActors() { return mActors; }
    std::vector<SolidCollider*>& getAllSolids() { return mSolids; }

private:
    CollisionManager(){};

    CollisionManager(const CollisionManager&) = delete;
    void operator=(const CollisionManager&) = delete;

    std::vector<ActorCollider*> mActors;
    std::vector<SolidCollider*> mSolids;
};

}  // namespace whal
