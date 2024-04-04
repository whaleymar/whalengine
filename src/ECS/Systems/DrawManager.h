#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct Draw;

class DrawManager : public ecs::ISystem<Draw> {
public:
    void onRemove(ecs::Entity entity) override;
};

}  // namespace whal
