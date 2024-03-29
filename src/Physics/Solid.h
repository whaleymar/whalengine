#pragma once

#include <vector>
#include "Physics/IUseCollision.h"
#include "Util/Types.h"

namespace whal {

class ActorCollider;

class SolidCollider : public IUseCollision {
public:
    SolidCollider(Vector2f position, Vector2i half);

    void move(f32 x, f32 y);
    std::vector<ActorCollider*> getRidingActors() const;

private:
    void moveDirection(f32 toMove, bool isXDirection, f32 solidEdge, EdgeGetter edgeFunc, std::vector<ActorCollider*>& riding);
};

}  // namespace whal
