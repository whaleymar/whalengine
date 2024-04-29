#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct Follow {
    Follow() = default;
    Follow(ecs::Entity target, ecs::Entity self);

    ecs::Entity target;
};

// give system which deletes children in ondelete
struct Children {
    std::vector<ecs::Entity> entities;
};

}  // namespace whal
