#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct RailsControl;
struct Velocity;
struct Position;

class RailsSystem : public ecs::ISystem<RailsControl, Position, Velocity> {
public:
    void update() override;
};

}  // namespace whal
