#pragma once

#include <chrono>
#include "Util/Types.h"

namespace whal {

class Deltatime {
public:
    static Deltatime& getInstance() {
        static Deltatime instance;
        return instance;
    }

    f32 get() const { return mDeltatime; }
    void update();

private:
    Deltatime();
    Deltatime(const Deltatime&) = delete;
    void operator=(const Deltatime&) = delete;

    std::chrono::time_point<std::chrono::steady_clock> mPreviousTime;
    f32 mDeltatime;
};

}  // namespace whal
