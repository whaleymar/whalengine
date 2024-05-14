#pragma once

#include "Util/Expected.h"
#include "Util/Vector.h"

namespace whal {

namespace ecs {
class Entity;
}

Expected<ecs::Entity> makeProjectile(Vector2i position, Vector2f velocity);

}  // namespace whal
