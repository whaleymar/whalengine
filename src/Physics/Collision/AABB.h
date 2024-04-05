#pragma once

#include <optional>
#include "Gfx/VertexObject.h"
#include "Physics/Collision/HitInfo.h"
#include "Physics/Collision/Segment.h"
#include "Util/Vector.h"

namespace whal {

struct AABB {
    Vector2i mCenter;
    Vector2i mHalf;

    // TODO check debug compile flag
    Vao vao;
    Vbo vbo;

    AABB(Vector2i half);
    AABB(Vector2i center, Vector2i half);
    void setPosition(Vector2i position);
    void setPosition(Vector2f position);
    const std::optional<HitInfo> collide(AABB other) const;
    const std::optional<HitInfo> collide(Segment segment) const;
    bool isOverlapping(AABB other) const;

    Vector2i getPosition() const { return mCenter; }
    f32 top() const { return mCenter.y() + mHalf.y(); }
    f32 bottom() const { return mCenter.y() - mHalf.y(); }
    f32 right() const { return mCenter.x() + mHalf.x(); }
    f32 left() const { return mCenter.x() - mHalf.x(); }
};

using EdgeGetter = f32 (AABB::*)() const;

}  // namespace whal
