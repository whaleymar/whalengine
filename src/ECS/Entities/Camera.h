#pragma once

#include "Util/Expected.h"

namespace whal {

namespace ecs {
class Entity;
}

Expected<ecs::Entity> createCamera(ecs::Entity target);

}  // namespace whal
