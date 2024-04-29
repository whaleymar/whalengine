#pragma once

#include "Util/Expected.h"

namespace whal {

namespace ecs {
class Entity;
}

Expected<ecs::Entity> createCamera();

}  // namespace whal
