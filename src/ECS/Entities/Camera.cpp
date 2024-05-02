#include "Camera.h"

#include "ECS/Components/Relationships.h"
#include "ECS/Components/Tags.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Lib/ECS.h"

namespace whal {

Expected<ecs::Entity> createCamera(ecs::Entity target) {
    auto& ecs = ecs::ECS::getInstance();

    auto expected = ecs.entity();
    if (!expected.isExpected()) {
        return expected;
    }

    auto camera = expected.value();
    camera.add(target.get<Transform>());
    camera.add(Follow(target));
    camera.add<Velocity>();
    camera.add<Camera>();

    return camera;
}

}  // namespace whal
