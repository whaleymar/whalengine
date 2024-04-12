#pragma once

#include "Util/Expected.h"

namespace whal {

namespace ecs {
class Entity;
}

struct Position;
struct Draw;
struct Sprite;

Expected<ecs::Entity> createBlock(Position position);
Expected<ecs::Entity> createBlock(Position position, Draw draw);
Expected<ecs::Entity> createBlock(Position position, Sprite sprite);

}  // namespace whal
