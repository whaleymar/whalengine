#pragma once

#include <optional>
#include "ECS/Components/Transform.h"
#include "Physics/Collision/HitInfo.h"
#include "Physics/Collision/Segment.h"
#include "Util/Vector.h"

#ifndef NDEBUG
#include "Gfx/VertexObject.h"
#endif

namespace whal {

struct AABB {
    AABB() = default;
    Vector2i center;
    Vector2i half;

#ifndef NDEBUG
    Vao vao;
    Vbo vbo;
#endif

    AABB(Vector2i half);
    AABB(Vector2i center, Vector2i half);
    AABB(Transform transform, Vector2i half);
    void setPosition(Vector2i position);
    void setPosition(Vector2f position);
    void setPositionFromBottom(Vector2i position);
    void setHalflen(Vector2i half);
    const std::optional<HitInfo> collide(const AABB& other) const;
    const std::optional<HitInfo> collide(Segment segment) const;
    bool isOverlapping(const AABB& other) const;
    Vector2i getPositionEdge(Vector2i unitDir) const;

    Vector2i getPosition() const { return center; }
    f32 top() const { return center.y() + half.y(); }
    f32 bottom() const { return center.y() - half.y(); }
    f32 right() const { return center.x() + half.x(); }
    f32 left() const { return center.x() - half.x(); }
};

using EdgeGetter = f32 (AABB::*)() const;

}  // namespace whal
