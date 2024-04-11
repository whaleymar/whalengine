#include "ECS/Lib/ECS.h"
#include "Util/Expected.h"

namespace whal::ecs {

Entity::Entity(EntityID id) : mId(id), mIsAlive(true){};

Expected<Entity> Entity::copy() const {
    return ECS::getInstance().copy(*this);
}

void Entity::kill() {
    ECS::getInstance().kill(*this);
    mIsAlive = false;
}

bool Entity::isAlive() const {
    return mIsAlive;
}

}  // namespace whal::ecs
