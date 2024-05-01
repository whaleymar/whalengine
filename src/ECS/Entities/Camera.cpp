#include "Camera.h"

#include "ECS/Components/Relationships.h"
#include "ECS/Components/Tags.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Lib/ECS.h"
#include "Util/Print.h"

namespace whal {

Expected<ecs::Entity> createCamera(ecs::Entity target) {
    auto& ecs = ecs::ECS::getInstance();

    auto expected = ecs.entity();
    if (!expected.isExpected()) {
        return expected;
    }
    print("created camera with id", expected.value().id());

    auto camera = expected.value();
    camera.add(target.get<Transform>());
    camera.add(Follow(target, camera));
    camera.add<Velocity>();
    camera.add<Camera>();

    // if (camera.has<Camera>()) {
    //     print("has camera");
    // }
    // if (camera.has<Transform>()) {
    //     print("has trasnform");
    // }

    return camera;
}

}  // namespace whal
