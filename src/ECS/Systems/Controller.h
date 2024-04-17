#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct PlayerControlRB;
struct PlayerControlFree;
struct RigidBody;
struct Transform;
struct Velocity;

class ControllerSystemRB : public ecs::ISystem<PlayerControlRB, Transform, Velocity, RigidBody> {
public:
    void update() override;
};

class ControllerSystemFree : public ecs::ISystem<PlayerControlFree, Transform, Velocity> {
public:
    void update() override;
};

}  // namespace whal
