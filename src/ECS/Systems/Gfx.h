#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct Position;
struct Draw;

class GraphicsSystem : public ecs::ISystem<Position, Draw> {
public:
    void update() override;
    void drawEntities();
};

}  // namespace whal
