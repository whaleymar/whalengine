#pragma once

#include "Util/Expected.h"

namespace whal {

namespace ecs {
class Entity;
}
struct Sprite;

Expected<ecs::Entity> createPlayer();

// use sprite with pre-constructed vao/vbo created on main thread
// TODO check if this is still necessary since I've moved vao/vbo initialization to System.onAdd
void createPlayerAsynch(Sprite sprite);

}  // namespace whal
