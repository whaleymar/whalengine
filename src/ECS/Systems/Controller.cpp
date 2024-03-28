#include "Controller.h"

#include "Systems/InputHandler.h"

namespace whal {

void ControllerSystem::update() {
    auto& input = Input::getInstance();
    for (auto& [entityid, entity] : getEntities()) {
        Vector2i delta;
        if (input.isLeft()) {
            delta += Vector::unitiLeft;
        }
        if (input.isRight()) {
            delta += Vector::unitiRight;
        }
        if (input.isUp()) {
            delta += Vector::unitiUp;
        }
        if (input.isDown()) {
            delta += Vector::unitiDown;
        }

        // TODO compare speed vs calling constructor
        // Position& pos = entity.get<Position>();
        // pos.e = pos.e + Vector2i(0, 1);

        entity.set(Position(delta + entity.get<Position>().e));
    }
}

}  // namespace whal
