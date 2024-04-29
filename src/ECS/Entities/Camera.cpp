#include "Camera.h"

#include "ECS/Components/Relationships.h"
#include "ECS/Components/Tags.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Lib/ECS.h"
#include "ECS/Systems/TagTrackers.h"

namespace whal {

Expected<ecs::Entity> createCamera() {
    auto& ecs = ecs::ECS::getInstance();

    auto expected = ecs.entity();
    if (!expected.isExpected()) {
        return expected;
    }

    auto camera = expected.value();
    if (PlayerSystem::instance()->getEntities().empty()) {
        return Error("Could not find any entities with Player tag");
    }
    ecs::Entity player = PlayerSystem::instance()->getEntities()[0];
    camera.add<Camera>();
    camera.add(Follow(player, camera));
    camera.add(player.get<Transform>());
    camera.add<Velocity>();

    return camera;
}

}  // namespace whal
