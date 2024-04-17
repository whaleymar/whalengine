#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct Position;
struct Velocity;

static constexpr f32 TERMINAL_VELOCITY_Y = -20;

// class PhysicsSystem : public ecs::ISystem<Position, Velocity, RigidBody> {
class PhysicsSystem : public ecs::ISystem<Position, Velocity> {
public:
    void update() override;
};

}  // namespace whal
