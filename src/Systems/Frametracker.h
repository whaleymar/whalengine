#pragma once

#include <chrono>

#include "Util/Types.h"

namespace whal {

class Frametracker {
public:
    static Frametracker& getInstance() {
        static Frametracker instance;
        return instance;
    }

    s32 getFrame() const { return mFrame; }
    f32 getFPS() const { return mFPS; }
    void update();

private:
    Frametracker();
    Frametracker(const Frametracker&) = delete;
    void operator=(const Frametracker&) = delete;

    s32 mFrame = 0;
    const s32 mMaxCount = 60;
    f32 mFPS = 0;
    std::chrono::time_point<std::chrono::steady_clock> mPreviousFrame1Time;
};

}  // namespace whal
