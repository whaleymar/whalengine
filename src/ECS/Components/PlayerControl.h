#pragma once

// #include "al/Nerve/NerveExecutor.h"

namespace whal {

// can't inherit from NerveExecutor because it has an explicit destructor -> no copy
// can't allocate pointer because I would need a destructor
// should create a singleton controller and point to that instead I guess?
struct PlayerControl {
    PlayerControl();
};

}  // namespace whal
