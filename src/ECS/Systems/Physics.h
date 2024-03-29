#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct Position;
struct Velocity;

// class PhysicsSystem : public ecs::ISystem<Position, Velocity, RigidBody> {
class PhysicsSystem : public ecs::ISystem<Position, Velocity> {
public:
    void update() override;
};

}  // namespace whal
