#pragma once

#include "ECS/Lib/ECS.h"
#include "Gfx/Shader.h"
#include "Gfx/VertexObject.h"

namespace whal {

class DebugCollisionViewer {
public:
    DebugCollisionViewer() = default;

    void draw(ShaderProgram program, ecs::Entity entity) const;

private:
    Vao mVao;
    Vbo mVbo;
};

}  // namespace whal
