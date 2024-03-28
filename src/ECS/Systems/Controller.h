#pragma once

#include "ECS/Components/PlayerControl.h"
#include "ECS/Components/Position.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Lib/ECS.h"

namespace whal {

class ControllerSystem : public ecs::ISystem<PlayerControl, Position, Velocity> {
public:
    void update() override;
};

}  // namespace whal
