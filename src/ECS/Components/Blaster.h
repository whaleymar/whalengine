#pragma once

#include "ECS/Lib/ECS.h"
#include "Gfx/GfxUtil.h"
// #include "Systems/Event.h"

// TODO game specific components/systems should go in another folder

namespace whal {
struct Transform;
}

struct Blaster {
    f32 projectileSpeed = 160;  // same as terminal velocity
    f32 shotKnockback = 50;
    f32 projectileLifetimeSeconds = 10;
    f32 explosionRadius = whal::PIXELS_PER_TILE * 1.5;
};

void onBlasterFired();

// TODO should require a generic PlayerControl component too, and can have a separate system for NPCs -- see note in Blaster.cpp
class ProjectileSystem : public whal::ecs::ISystem<Blaster, whal::Transform> {
public:
    ProjectileSystem();

private:
    whal::EventListener<whal::Vector2i> mBlasterEventListener;
};
