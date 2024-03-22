#pragma once

#include <array>
#include <queue>

#include "ECS/Lib/Component.h"
#include "ECS/Lib/Entity.h"
#include "Util/Expected.h"
#include "Util/Types.h"

namespace whal::ecs {

// TODO a way to temporarily deactivate a component without changing the pattern?
class EntityManager {
public:
    EntityManager();

    Expected<Entity> createEntity();
    void destroyEntity(Entity entity);
    void setPattern(Entity entity, Pattern pattern);
    Pattern getPattern(Entity entity) const;

private:
    // ? priority queue to ensure entities are packed? makes insert + delete nlogn
    std::queue<EntityID> mAvailableIDs;
    std::array<Pattern, MAX_ENTITIES> mPatterns;
    u32 mEntityCount = 0;
};

}  // namespace whal::ecs
