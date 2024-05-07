#pragma once
#include "ECS/Lib/ECS.h"
namespace whal {

struct OnFrameEnd;

class OnFrameEndSystem : public ecs::ISystem<OnFrameEnd> {
public:
    void update() override;
};

}  // namespace whal
