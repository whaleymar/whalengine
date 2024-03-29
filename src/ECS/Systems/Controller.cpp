#include "Controller.h"

#include "ECS/Components/PlayerControl.h"
#include "ECS/Components/Velocity.h"

#include "Systems/InputHandler.h"

namespace whal {

void ControllerSystem::update() {
    auto& input = Input::getInstance();
    for (auto& [entityid, entity] : getEntities()) {
        Vector2f delta;
        if (input.isLeft()) {
            delta += Vector::unitfLeft;
        }
        if (input.isRight()) {
            delta += Vector::unitfRight;
        }
        if (input.isUp()) {
            delta += Vector::unitfUp;
        }
        if (input.isDown()) {
            delta += Vector::unitfDown;
        }

        // TODO compare speed vs calling constructor
        // Position& pos = entity.get<Position>();
        // pos.e = pos.e + Vector2i(0, 1);

        PlayerControl& control = entity.get<PlayerControl>();
        delta *= control.moveSpeed;

        // TODO how to add to velocity without 1. killing all momentum and 2. without infinite speed and 3. without a hard speed cap
        // --> maybe a controller-only speed cap? only add up to max value
        // entity.set(Velocity(delta + entity.get<Velocity>().e));
        Velocity vel = Velocity(delta);
        entity.set(vel);

        // Velocity& vel = entity.get<Velocity>();
        // std::cout << vel.e.x() << " " << vel.e.y() << std::endl;
        // std::cout << delta.x() << " " << delta.y() << std::endl;
    }
}

}  // namespace whal
