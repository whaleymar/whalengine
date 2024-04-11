#include "Animator.h"

#include <cstring>

#include "ECS/Lib/ECS.h"
#include "Gfx/Texture.h"
#include "Systems/Deltatime.h"

namespace whal {

bool basicAnimation(Animator& animator, ecs::Entity entity) {
    Animation& anim = animator.getAnimation();

    animator.curFrameDuration += Deltatime::getInstance().get();
    if (animator.curFrameDuration >= anim.secondsPerFrame) {
        animator.nextFrame();
        return true;
    }
    return false;
}

Animator::Animator(std::vector<Animation> animations_, AnimBrain brain_) : animations(animations_), brain(brain_) {}

Frame Animator::getFrame() const {
    return animations[curAnimIx].getFrame(curFrameIx);
}

Animation& Animator::getAnimation() {
    return animations[curAnimIx];
}

void Animator::setAnimation(const char* name) {
    s32 i = 0;
    for (auto& animation : animations) {
        if (strcmp(animation.name, name) == 0) {
            curAnimIx = i;
            return;
        }
        i++;
    }
}

void Animator::nextFrame() {
    curFrameIx = (curFrameIx + 1) % getAnimation().getFrameCount();
    curFrameDuration = 0.0;
}

void Animator::resetAnimation() {
    curFrameIx = 0;
    curFrameDuration = 0;
}

Animation::Animation() : name(""), frames({}){};

Animation::Animation(const char* name_, std::vector<Frame> frames_, f32 secondsPerFrame_)
    : name(name_), frames(frames_), secondsPerFrame(secondsPerFrame_) {}

Frame Animation::getFrame(s32 ix) const {
    return frames[ix];
}

s32 Animation::getFrameCount() const {
    return frames.size();
}

}  // namespace whal
