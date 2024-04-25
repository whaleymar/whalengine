#include "Lifetime.h"
#include "ECS/Components/Lifetime.h"
#include "Util/Print.h"

namespace whal {

void LifetimeSystem::update() {
    f32 dt = System::dt();
    // TODO this will mutate the entity hashmap while iterating --> error?
    for (auto [entityid, entity] : getEntities()) {
        auto& lifetime = entity.get<Lifetime>();
        lifetime.secondsRemaining -= dt;
        if (lifetime.secondsRemaining <= 0) {
            // print("killing entity", entityid);
            entity.kill();
        }
    }
}

}  // namespace whal
