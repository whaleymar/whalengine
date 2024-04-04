#pragma once

#include <vector>

namespace whal {

class ActorCollider;
class SolidCollider;

class CollisionManager {
public:
    static CollisionManager& getInstance() {
        static CollisionManager instance;
        return instance;
    }

    const std::vector<ActorCollider*>& getAllActors() const { return mActors; }
    const std::vector<SolidCollider*>& getAllSolids() const { return mSolids; }
    void add(ActorCollider* actor);
    void add(SolidCollider* solid);
    void remove(ActorCollider* actor);
    void remove(SolidCollider* solid);
    void clearActors();
    void clearSolids();
    void setDebug(bool debug);
    bool isDebugActive() const { return mIsDebugActive; }

private:
    CollisionManager() = default;

    CollisionManager(const CollisionManager&) = delete;
    void operator=(const CollisionManager&) = delete;

    std::vector<ActorCollider*> mActors{};
    std::vector<SolidCollider*> mSolids{};
    bool mIsDebugActive = false;
};

}  // namespace whal
