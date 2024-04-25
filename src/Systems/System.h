#pragma once

#include "Systems/Audio.h"
#include "Systems/Deltatime.h"
#include "Systems/Event.h"
#include "Systems/Frametracker.h"
#include "Systems/InputHandler.h"
#include "Systems/JobScheduler.h"
#include "Systems/Random.h"

namespace whal {

struct System {
    inline static InputHandler input;
    inline static Deltatime dt;
    inline static RNG rng;
    inline static Frametracker frame;
    inline static EventManager eventMgr;
    inline static AudioPlayer audio;
    inline static JobScheduler schedule;
};

}  // namespace whal
