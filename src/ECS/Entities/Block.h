#pragma once

#include "Util/Expected.h"

namespace whal {

namespace ecs {
class Entity;
}

struct Transform;
struct Draw;
struct Sprite;

Expected<ecs::Entity> createBlock(Transform transform);
Expected<ecs::Entity> createBlock(Transform transform, Draw draw);
Expected<ecs::Entity> createBlock(Transform transform, Sprite sprite);

}  // namespace whal
