#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct Animator;
struct Sprite;

class AnimationSystem : public ecs::ISystem<Animator, Sprite> {
public:
    void update() override;
};

}  // namespace whal
