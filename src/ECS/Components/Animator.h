#pragma once

#include <vector>

#include "Util/Types.h"

namespace whal {

namespace ecs {
class Entity;
}

struct Animator;
struct Animation;
struct Frame;

using AnimBrain = bool (*)(Animator& animator, ecs::Entity entity);

bool basicAnimation(Animator& animator, ecs::Entity entity);

/*
 * the animator controls which animation an entity is using and handles frame-advancing
 * it contains a list of animations + a "brain" function which handles state changes
 *
 * constraint: all animations have the same dimensions
 */
struct Animator {
    Animator() = default;
    Animator(std::vector<Animation> animations_, AnimBrain brain_);

    std::vector<Animation> animations;
    AnimBrain brain = &basicAnimation;
    s32 curAnimIx = 0;
    s32 curAnimDuration = 0.0;
    s32 curFrameIx = 0;
    f32 curFrameDuration = 0.0;

    Frame getFrame() const;
    Animation& getAnimation();
    bool setAnimation(const char* name);
    void nextFrame();
    void resetAnimation();
};

// an animation is a sequence of same-sized frames
struct Animation {
    Animation();
    Animation(const char* name, std::vector<Frame> frames, f32 secondsPerFrame = 0.25);

    Frame getFrame(s32 ix) const;
    s32 getFrameCount() const;

    const char* name;
    std::vector<Frame> frames;
    f32 secondsPerFrame = 0.25;
};

}  // namespace whal
