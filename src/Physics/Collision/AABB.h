#pragma once

#include <optional>
#include "Physics/Collision/HitInfo.h"
#include "Physics/Collision/Segment.h"
#include "Util/Vector.h"

#ifndef NDEBUG
#include "Gfx/VertexObject.h"
#endif

namespace whal {

struct AABB {
    Vector2i mCenter;
    Vector2i mHalf;

#ifndef NDEBUG
    Vao vao;
    Vbo vbo;
#endif

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
