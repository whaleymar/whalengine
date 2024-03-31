#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct PlayerControlRB;
struct PlayerControlFree;
struct RigidBody;
struct Position;
struct Velocity;

class ControllerSystemRB : public ecs::ISystem<PlayerControlRB, Position, Velocity, RigidBody> {
public:
    void update() override;
};

class ControllerSystemFree : public ecs::ISystem<PlayerControlFree, Position, Velocity> {
public:
    void update() override;
};

}  // namespace whal
