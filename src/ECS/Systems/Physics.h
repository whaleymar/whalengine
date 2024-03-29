#pragma once

#include "ECS/Components/Position.h"
// #include "ECS/Components/RigidBody.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Lib/ECS.h"

namespace whal {

// class PhysicsSystem : public ecs::ISystem<Position, Velocity, RigidBody> {
class PhysicsSystem : public ecs::ISystem<Position, Velocity> {
public:
    void update() override;
};

}  // namespace whal
