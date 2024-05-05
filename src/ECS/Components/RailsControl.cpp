#include "RailsControl.h"
#include "Gfx/GfxUtil.h"

namespace whal {

constexpr f32 SPEED_CURVE_EPSILON = 0.01;

RailsControl::RailsControl(f32 moveSpeed_, std::vector<CheckPoint> checkPoints_, f32 waitTime_, bool isCycle_, ArrivalCallback callback)
    : mCheckpoints(checkPoints_), speed(moveSpeed_), waitTime(waitTime_), arrivalCallback(callback), curActionTime(waitTime_), isCycle(isCycle_) {}

void RailsControl::setCheckpoints(std::vector<CheckPoint>& checkpoints) {
    mCheckpoints = std::move(checkpoints);
}

RailsControl::CheckPoint RailsControl::getTarget() const {
    return mCheckpoints[curTarget];
}

void RailsControl::startManually() {
    isVelocityUpdateNeeded = true;
}

void RailsControl::step() {
    startPosition = toFloatVec(getTarget().position);
    curTarget++;
    if (curTarget == mCheckpoints.size()) {
        curTarget = 0;
    }
}

f32 RailsControl::getSpeed(Vector2i currentPosition) {
    // this is super hacky
    // because my easeIn/easeOut functions describe 2nd order functions
    // but i need to set a velocity, not position, so the physics system will work
    // So I calculate the position I should be in, according to the movement function.
    // Then I use the current position to get the velocity I should go to get there by the next frame.
    //
    // I feel like I should be using deltatime but it breaks things
    // but idk it Just Works

    // TODO use derivates of easein/out functions

    const Vector2f targetPosf = toFloatVec(getTarget().position);

    const f32 segmentDistance = (targetPosf - startPosition).len();
    const f32 expectedSegmentTime = segmentDistance / (speed * static_cast<f32>(PIXELS_PER_TILE));  // speed is in tiles/sec, but pos is in pixels

    f32 t = curActionTime / expectedSegmentTime;
    f32 progress;
    switch (getTarget().movement) {
    case Movement::LINEAR:
        progress = t;
        break;

    case Movement::EASEIO_SINE:
        progress = easeInOutSine(0, 1, t);
        break;

    case Movement::EASEIO_BEZIER:
        progress = easeInOutBezier(0, 1, t);
        break;

    case Movement::EASEI_QUAD:
        progress = easeInQuad(0, 1, t);
        break;

    case Movement::EASEI_CUBE:
        progress = easeInCubic(0, 1, t);
        break;

    case Movement::EASEO_QUAD:
        progress = easeOutQuad(0, 1, t);
        break;

    case Movement::EASEO_CUBE:
        progress = easeOutCubic(0, 1, t);
        break;
    }

    if ((1 - progress) < SPEED_CURVE_EPSILON) {
        isVelocityUpdateNeeded = false;
    }
    const Vector2f newPos = lerp(startPosition, targetPosf, progress);
    return (newPos - toFloatVec(currentPosition)).len();
}

f32 RailsControl::getSpeedNew() {
    // this isn't working for some reason
    // these can be calced once per segment
    const Vector2f targetPosf = toFloatVec(getTarget().position);
    const f32 segmentDistance = (targetPosf - startPosition).len();
    const f32 expectedSegmentTime = segmentDistance / (speed * static_cast<f32>(PIXELS_PER_TILE));  // speed is in tiles/sec, but pos is in pixels

    f32 t = curActionTime / expectedSegmentTime;
    f32 newSpeed;
    switch (getTarget().movement) {
    case Movement::LINEAR:
        newSpeed = speed;
        break;

    case Movement::EASEIO_SINE:
        newSpeed = easeInOutSineDt(0, speed, t);
        break;

    case Movement::EASEIO_BEZIER:
        newSpeed = easeInOutBezierDt(0, speed, t);
        break;

    case Movement::EASEI_QUAD:
        newSpeed = easeInQuadDt(0, speed, t);
        break;

    case Movement::EASEI_CUBE:
        newSpeed = easeInCubicDt(0, speed, t);
        break;

    case Movement::EASEO_QUAD:
        newSpeed = easeOutQuad(0, 1, t);
        break;

    case Movement::EASEO_CUBE:
        newSpeed = easeOutCubic(0, 1, t);
        break;
    }

    return newSpeed;
}

bool RailsControl::isValid() const {
    return mCheckpoints.size() > 1;
}

}  // namespace whal
