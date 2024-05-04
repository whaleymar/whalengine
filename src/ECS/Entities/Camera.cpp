#include "Camera.h"

#include "ECS/Components/Name.h"
#include "ECS/Components/RailsControl.h"
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
    camera.add(Name("Camera"));
    camera.add<Camera>();

    return camera;
}

void onCameraAtDestination(ecs::Entity cameraEntity, RailsControl& rails) {
    cameraEntity.remove<RailsControl>();
    cameraEntity.add<Velocity>();  // railscontrol removed it
}

RailsControl createCameraMoveController(Vector2i currentPosition, Vector2i nextPosition) {
    return RailsControl(45,
                        {
                            {currentPosition, RailsControl::Movement::LINEAR},
                            {nextPosition, RailsControl::Movement::EASEO_CUBE},
                        },
                        0, true, &onCameraAtDestination);
}

}  // namespace whal
