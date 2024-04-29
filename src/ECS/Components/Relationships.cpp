#include "Relationships.h"

namespace whal {

Follow::Follow(ecs::Entity target_, ecs::Entity self) : target(target_) {
    if (target.has<Children>()) {
        target.get<Children>().entities.push_back(self);
        return;
    }
    target.add(Children({self}));
}

}  // namespace whal
