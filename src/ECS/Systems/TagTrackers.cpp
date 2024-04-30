#include "TagTrackers.h"

#include "ECS/Components/Transform.h"
#include "ECS/Lib/ECS.h"

namespace whal {

std::optional<ecs::Entity> getCamera() {
    if (CameraSystem::instance()->getEntities().empty()) {
        return std::nullopt;
    }
    return CameraSystem::instance()->getEntities()[0];
}

Vector2i getCameraPosition() {
    static Vector2i lastPos;
    auto eOpt = getCamera();
    if (eOpt) {
        // i have no idea why this fails // TODO
        auto lastPosOpt = eOpt.value().tryGet<Transform>();
        if (lastPosOpt) {
            lastPos = lastPosOpt.value()->position;
        }
    }
    return lastPos;
}

}  // namespace whal
