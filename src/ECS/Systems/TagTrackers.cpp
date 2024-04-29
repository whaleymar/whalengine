#include "TagTrackers.h"

namespace whal {

std::optional<ecs::Entity> getCamera() {
    if (CameraSystem::instance()->getEntities().empty()) {
        return std::nullopt;
    }
    return CameraSystem::instance()->getEntities()[0];
}

}  // namespace whal
