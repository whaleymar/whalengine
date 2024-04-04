#include "DrawManager.h"

#include "ECS/Components/Draw.h"

namespace whal {

void DrawManager::onRemove(ecs::Entity entity) {
    entity.get<Draw>().vao.free();
    entity.get<Draw>().vbo.free();
}

}  // namespace whal
