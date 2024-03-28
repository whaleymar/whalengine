#pragma once

#include "ECS/Components/Position.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Lib/ECS.h"

namespace whal {

class PhysicsSystem : public ecs::ISystem<Position, Velocity, RigidBody> {
public:
    void update() override;
};

}  // namespace whal
