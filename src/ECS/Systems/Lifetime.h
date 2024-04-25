#pragma once

#include "ECS/Lib/ECS.h"
namespace whal {

struct Lifetime;

class LifetimeSystem : public ecs::ISystem<Lifetime> {
public:
    void update() override;
};

}  // namespace whal
