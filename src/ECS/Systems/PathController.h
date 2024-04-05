#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct PathControl;
struct Velocity;
struct Position;

// TODO rename this to "RailsControl" or something like that?
class PathControllerSystem : public ecs::ISystem<PathControl, Position, Velocity> {
public:
    void update() override;
};

}  // namespace whal
