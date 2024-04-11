#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct Animator;
struct Sprite;
struct Position;

class AnimationSystem : public ecs::ISystem<Animator, Sprite, Position> {
public:
    void update() override;
};

}  // namespace whal
