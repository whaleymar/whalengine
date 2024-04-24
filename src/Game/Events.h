#pragma once

#include "Systems/Event.h"

namespace whal {

namespace ecs {
class Entity;
}

// Events I can think of
// (player) spawn event

class DeathEvent : public IEvent<ecs::Entity> {};
class CollisionEvent : public IEvent<ecs::Entity, ecs::Entity> {};

namespace Event {

inline const DeathEvent DEATH_EVENT;
inline const CollisionEvent COLLISION_EVENT;

}  // namespace Event

}  // namespace whal
