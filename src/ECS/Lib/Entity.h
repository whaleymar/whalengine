#pragma once

#include "Util/Expected.h"
#include "Util/Types.h"

namespace whal::ecs {

class EntityManager;
class SystemManager;

using EntityID = u32;
const u32 MAX_ENTITIES = 5000;

class Entity {
public:
    Entity() = default;
    Entity(EntityID id);
    friend EntityManager;
    friend SystemManager;
    friend Expected<Entity>;  // create dummy id on error
    EntityID id() const { return mId; }

    EntityID operator()() const { return mId; }
    bool operator==(Entity other) const { return mId == other.mId; }

private:
    EntityID mId = 0;
};
}  // namespace whal::ecs
