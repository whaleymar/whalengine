#include "Physics/Collision/AABB.h"

#include "Util/MathUtil.h"
#include "Util/Vector.h"

namespace whal {

AABB::AABB(Vector2i half) : mCenter(Vector::zero2i), mHalf(half) {}
AABB::AABB(Vector2i center, Vector2i half) : mCenter(center), mHalf(half) {}

void AABB::setPosition(Vector2i position) {
    mCenter = position;
}

void AABB::setPosition(Vector2f position) {
    mCenter = Vector2i(static_cast<s32>(position.x()), static_cast<s32>(position.y()));
}

const std::optional<HitInfo> AABB::collide(AABB other) const {
    const auto delta = other.mCenter - mCenter;
    const auto overlap = mHalf + other.mHalf;

    const s32 px = overlap.x() - abs(delta.x());
    if (px <= 0) {
        return std::nullopt;
    }

    const s32 py = overlap.y() - abs(delta.y());
    if (py <= 0) {
        return std::nullopt;
    }

    if (px == py) {
        const s32 signX = sign(delta.x());
        const s32 signY = sign(delta.y());
        Vector2i hitPos(mCenter.x() + mHalf.x() * signX, mCenter.y() + mHalf.y() * signY);
        Vector2i hitDelta(px * signX, py * signY);
        Vector2i hitNormal(signX, signY);
        return HitInfo(hitPos, hitDelta, hitNormal);
    }
    if (px < py) {
        const s32 signX = sign(delta.x());
        Vector2i hitPos(mCenter.x() + mHalf.x() * signX, other.mCenter.y());
        Vector2i hitDelta(px * signX, 0);
        Vector2i hitNormal(signX, 0);
        return HitInfo(hitPos, hitDelta, hitNormal);
    } else {
        const s32 signY = sign(delta.y());
        Vector2i hitPos(other.mCenter.x(), mCenter.y() + mHalf.y() * signY);
        Vector2i hitDelta(0, py * signY);
        Vector2i hitNormal(0, signY);
        return HitInfo(hitPos, hitDelta, hitNormal);
    }
}

const std::optional<HitInfo> AABB::collide(Segment segment) const {
    const s32 scaleX = 1 / segment.delta.x();
    const s32 scaleY = 1 / segment.delta.y();
    const s32 signX = sign(segment.delta.x());
    const s32 signY = sign(segment.delta.y());

    const s32 nearX = (mCenter.x() - signX * mHalf.x() - segment.origin.x()) * scaleX;
    const s32 nearY = (mCenter.y() - signY * mHalf.y() - segment.origin.y()) * scaleY;

    const s32 farX = (mCenter.x() + signX * mHalf.x() - segment.origin.x()) * scaleX;
    const s32 farY = (mCenter.y() + signY * mHalf.y() - segment.origin.y()) * scaleY;

    if (nearX > farY || nearY > farX) {
        // segment missed the AABB
        return std::nullopt;
    }

    const s32 near = nearX > nearY ? nearX : nearY;
    const s32 far = farX < farY ? farX : farY;
    if (near >= 1 || far <= 0) {
        // segment ends before reaching nearest edge
        // OR segment points away from AABB
        return std::nullopt;
    }

    // collision is happening
    // "If the near time is greater than zero, the segment starts outside and is entering the box. Otherwise, the segment starts inside the box, and
    // is exiting it. If we’re entering the box, we can set the hit time to the near time, since that’s the point along the segment at which it
    // collided. If it’s inside, it’s colliding at the very starting of the line, so just set the hit time to zero."
    Vector2i hitNormal = Vector::zero2i;
    if (nearX == nearY) {
        hitNormal.e[0] = -signX;
        hitNormal.e[1] = -signY;
    } else if (nearX > nearY) {
        hitNormal.e[0] = -signX;
    } else {
        hitNormal.e[1] = -signY;
    }
    const s32 uncollidedFraction = clamp(near, 0, 1);
    Vector2i hitDelta = segment.delta * (-1 * (1 - uncollidedFraction));
    Vector2i hitPos = segment.delta * uncollidedFraction + segment.origin;
    return HitInfo(hitPos, hitDelta, hitNormal);
}

bool AABB::isOverlapping(AABB other) const {
    const auto delta = other.mCenter - mCenter;
    const auto overlap = mHalf + other.mHalf;
    return overlap.x() > abs(delta.x()) || overlap.y() > abs(delta.y());
}

}  // namespace whal
