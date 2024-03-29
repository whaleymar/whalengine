#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct PathControl;
struct Velocity;
struct Position;

class PathControllerSystem : public ecs::ISystem<PathControl, Position, Velocity> {
public:
    void update() override;
};

}  // namespace whal
