#include "Lifetime.h"
#include "ECS/Components/Lifetime.h"

namespace whal {

void LifetimeSystem::update() {
    f32 dt = System::dt();
    for (auto [entityid, entity] : getEntitiesRef()) {
        auto& lifetime = entity.get<Lifetime>();
        lifetime.secondsRemaining -= dt;
        if (lifetime.secondsRemaining <= 0) {
            entity.kill();
        }
    }
}

}  // namespace whal
