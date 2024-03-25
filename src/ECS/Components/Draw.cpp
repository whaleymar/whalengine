#include "ECS/Components/Draw.h"
#include "Gfx/VertexObject.h"

namespace whal {

Draw::Draw(s32 depth_) : depth(depth_) {
    // vertices = MakeRectVertices(frameSize.x(), frameSize.y());
    vertices = MakeRectVertices(100, 100);
}

}  // namespace whal
