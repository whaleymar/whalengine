#include "PlayerControl.h"

// #include "Gfx/GfxUtil.h"

namespace whal {

PlayerControlRB::PlayerControlRB(f32 moveSpeed_, f32 jumpHeight_) : moveSpeed(moveSpeed_), jumpHeight(jumpHeight_){};
// PlayerControlRB::PlayerControlRB(f32 moveSpeed_, f32 jumpHeight_) : moveSpeed(moveSpeed_ / RES_FACTOR), jumpHeight(jumpHeight_){};

PlayerControlFree::PlayerControlFree(f32 moveSpeed_) : moveSpeed(moveSpeed_){};

}  // namespace whal
