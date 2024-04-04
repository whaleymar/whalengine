#include "CollisionManager.h"

#include <algorithm>

namespace whal {

void CollisionManager::add(ActorCollider* actor) {
    mActors.push_back(actor);
}

void CollisionManager::add(SolidCollider* solid) {
    mSolids.push_back(solid);
}

void CollisionManager::remove(ActorCollider* actor) {
    auto ix = std::find(mActors.begin(), mActors.end(), actor);
    if (ix != mActors.end()) {
        mActors.erase(ix);
    }
}

void CollisionManager::remove(SolidCollider* solid) {
    auto ix = std::find(mSolids.begin(), mSolids.end(), solid);
    if (ix != mSolids.end()) {
        mSolids.erase(ix);
    }
}

void CollisionManager::clearActors() {
    mActors.clear();
}

void CollisionManager::clearSolids() {
    mSolids.clear();
}

void CollisionManager::setDebug(bool debug) {
    mIsDebugActive = debug;
}

}  // namespace whal
