#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct PlayerControl;
struct Position;
struct Velocity;

class ControllerSystem : public ecs::ISystem<PlayerControl, Position, Velocity> {
public:
    void update() override;
};

}  // namespace whal
