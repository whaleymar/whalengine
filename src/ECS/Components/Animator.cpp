#include "Animator.h"

#include "ECS/Lib/ECS.h"
#include "Systems/Deltatime.h"

namespace whal {

bool basicAnimation(Animator& animator, ecs::Entity entity) {
    Animation& anim = animator.animations[0];

    anim.curFrameDuration += Deltatime::getInstance().get();
    if (anim.curFrameDuration >= anim.secondsPerFrame) {
        anim.next();
        return true;
    }
    return false;
}

Animator::Animator(std::vector<Animation> animations_) : animations(animations_) {}

Frame Animator::getFrame() const {
    return animations[curAnimIx].getFrame();
}

Animation::Animation() : name(""), nFrames(0), frames({}){};

Animation::Animation(const char* name_, std::vector<Frame> frames_) : name(name_), nFrames(frames_.size()), frames(frames_) {}

void Animation::next() {
    curFrameIx = (curFrameIx + 1) % nFrames;
    curFrameDuration = 0.0;
}

Frame Animation::getFrame() const {
    return frames[curFrameIx];
}

}  // namespace whal
