#pragma once

#include "ECS/Components/Draw.h"
#include "ECS/Components/Position.h"
#include "ECS/Lib/ECS.h"

namespace whal {

class ShaderProgram;

class GraphicsSystem : public ecs::ISystem<Position, Draw> {
public:
    void update() override;
    void drawEntities(ShaderProgram program);
};

}  // namespace whal
