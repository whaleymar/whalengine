#include <iostream>  // TODO don't include
#include "ECS/Components/Position.h"
#include "ECS/Lib/ECS.h"
#include "ECS/Lib/System.h"

namespace whal {

class PhysicsSystem : public ecs::ISystem<Position> {
public:
    void update() override {
        for (auto const& [entityid, entity] : getEntities()) {
            auto opt = ecs::ECS::getInstance().getComponent<Position>(entity);
            if (!opt) {
                std::cout << "entity with id " << entityid << " missing Position component" << std::endl;
                continue;
            }
            Position* pos = opt.value();
            // pos->e = pos->e + Vector2i(0, 1);
            pos->e = pos->e + Vector2i(0, 1);
        }
    }
};

}  // namespace whal
