#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct RailsControl;
struct Velocity;
struct Position;

class RailsSystem : public ecs::ISystem<RailsControl, Position> {
public:
    void update() override;
};

}  // namespace whal
