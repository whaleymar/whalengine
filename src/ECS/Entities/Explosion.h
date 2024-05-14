#pragma once

#include "Util/Expected.h"
#include "Util/Vector.h"

namespace whal::ecs {
class Entity;
}

Expected<whal::ecs::Entity> makeExplosionZone(whal::Vector2i center, s32 halflen);
