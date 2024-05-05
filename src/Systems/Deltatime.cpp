#include "Deltatime.h"

#include <chrono>

namespace whal {

Deltatime::Deltatime() : mPreviousTime(std::chrono::steady_clock::now()), mDeltatime(0.01) {}

void Deltatime::update() {
    auto newTime = std::chrono::steady_clock::now();
    mDeltatimeUnmodified = std::chrono::duration<f32>(newTime - mPreviousTime).count();
    mDeltatime = mDeltatimeUnmodified * mTimeMultiplier;
    mPreviousTime = newTime;
}

void Deltatime::setMultiplier(f32 multiplier) {
    mTimeMultiplier = multiplier;
}

}  // namespace whal
