#include "CallbackSystem.h"

#include "ECS/Components/Callback.h"

namespace whal {

void OnFrameEndSystem::update() {
    for (auto [entityid, entity] : getEntitiesCopy()) {
        // not bothering with a null check
        entity.get<OnFrameEnd>().callback(entity);
        entity.remove<OnFrameEnd>();
    }
}

}  // namespace whal
