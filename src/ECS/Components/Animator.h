#pragma once

#include <vector>

#include "Util/Vector.h"

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
    Animator(std::vector<Animation> animations_);

    std::vector<Animation> animations;
    s32 curAnimIx = 0;
    AnimBrain brain = &basicAnimation;

    Frame getFrame() const;
};

// an animation is a sequence of same-sized frames
struct Animation {
    Animation();
    Animation(const char* name, std::vector<Frame> frames);

    void next();
    Frame getFrame() const;

    const char* name;
    s32 nFrames;
    f32 secondsPerFrame = 0.25;
    std::vector<Frame> frames;
    s32 curFrameIx = 0;
    f32 curFrameDuration = 0.0;
};

struct Frame {
    Vector2i atlasPositionTexels;
    Vector2i dimensionsTexels;
};

}  // namespace whal
