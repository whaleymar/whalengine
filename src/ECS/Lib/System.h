#pragma once

#include <set>

#include "ECS/Lib/Entity.h"

namespace whal::ecs {

class SystemManager;

// each system has a red-black tree of entities it operates on
// currently their methods are called manually
class ISystem {
public:
    friend SystemManager;
    ISystem() { mEntities = {}; }

private:
    std::set<EntityID> mEntities;
};

}  // namespace whal::ecs
