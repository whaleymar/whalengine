#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct Transform;
struct Velocity;

inline constexpr f32 TERMINAL_VELOCITY_Y = -160;

class PhysicsSystem : public ecs::ISystem<Transform, Velocity> {
public:
    void update() override;
};

}  // namespace whal
