#include "Lifetime.h"
#include "ECS/Components/Lifetime.h"

namespace whal {

void LifetimeSystem::update() {
    f32 dt = System::dt();
    std::vector<ecs::Entity> toDelete;
    // TODO this will mutate the entity hashmap while iterating --> error?
    for (auto [entityid, entity] : getEntities()) {
        auto& lifetime = entity.get<Lifetime>();
        lifetime.secondsRemaining -= dt;
        if (lifetime.secondsRemaining <= 0) {
            // entity.kill();
            toDelete.push_back(entity);
        }
    }
    for (auto entity : toDelete) {
        entity.kill();
    }
}

}  // namespace whal
