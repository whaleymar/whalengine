#include "ECS/Lib/ECS.h"

namespace whal::ecs {

Entity::Entity(EntityID id) : mId(id){};

void Entity::kill() {
    ECS::getInstance().kill(*this);
}

}  // namespace whal::ecs
