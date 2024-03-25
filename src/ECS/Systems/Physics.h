#pragma once

#include "ECS/Components/Position.h"
#include "ECS/Lib/ECS.h"

namespace whal {

class PhysicsSystem : public ecs::ISystem<Position> {
public:
    void update() override {
        for (auto const& [entityid, entity] : getEntities()) {
            Position& pos = entity.get<Position>();
            pos.e = pos.e + Vector2i(0, 1);
        }
    }
};

}  // namespace whal
