#pragma once

#include "ECS/Lib/ECS.h"
#include "Systems/Event.h"

// TODO game specific components/systems should go somewhere else

namespace whal {
struct Transform;
}

struct Blaster {
    f32 projectileSpeed = 160;  // same as terminal velocity
};

void onBlasterFired();

// TODO should require a generic PlayerControl component too, and can have a separate system for NPCs
class ProjectileSystem : public whal::ecs::ISystem<Blaster, whal::Transform> {
public:
    ProjectileSystem();

private:
    whal::EventListener<whal::Vector2i> mBlasterEventListener;
};
