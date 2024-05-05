#pragma once

#include <chrono>
#include "Util/Types.h"

namespace whal {

struct System;

class Deltatime {
public:
    friend System;

    f32 operator()() const { return mDeltatime; }
    f32 getUnmodified() const { return mDeltatimeUnmodified; }
    void update();
    void setMultiplier(f32);

private:
    Deltatime();
    Deltatime(const Deltatime&) = delete;
    void operator=(const Deltatime&) = delete;

    std::chrono::time_point<std::chrono::steady_clock> mPreviousTime;
    f32 mDeltatime;
    f32 mDeltatimeUnmodified;
    f32 mTimeMultiplier = 1.0;
};

}  // namespace whal
