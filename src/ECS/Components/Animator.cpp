#include "Animator.h"

#include <cstring>

#include "ECS/Lib/ECS.h"
#include "Gfx/Texture.h"
#include "Systems/Deltatime.h"
#include "Util/Print.h"

namespace whal {

bool basicAnimation(Animator& animator, ecs::Entity entity) {
    Animation& anim = animator.getAnimation();

    f32 dt = Deltatime::getInstance().get();
    animator.curFrameDuration += dt;
    animator.curAnimDuration += dt;
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

// TODO switch to enums for comparisons in this func + animator setter
bool Animator::setAnimation(const char* name) {
    if (strcmp(getAnimation().name, name) == 0) {
        return false;
    }

    for (size_t i = 0; i < animations.size(); i++) {
        auto& animation = animations[i];
        if (strcmp(animation.name, name) == 0) {
            curAnimIx = i;
            resetAnimation();
            // print("set animation:", name);
            return true;
        }
    }
    print("failed to set animation:", name);
    return false;  // return error?
}

void Animator::nextFrame() {
    curFrameIx = (curFrameIx + 1) % getAnimation().getFrameCount();
    curFrameDuration = 0.0;
}

void Animator::resetAnimation() {
    curAnimDuration = 0;
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
