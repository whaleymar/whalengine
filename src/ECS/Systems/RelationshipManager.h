#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct Children;
struct Follow;
struct Velocity;
struct Transform;

// TODO needs onEntityDeath listener to remove child from Children.entities
// if it dies
class EntityChildSystem : public ecs::ISystem<Children> {
public:
    void onRemove(ecs::Entity entity) override;
};

class FollowSystem : public ecs::ISystem<Follow, Velocity, Transform> {
public:
    void update() override;
};

}  // namespace whal
