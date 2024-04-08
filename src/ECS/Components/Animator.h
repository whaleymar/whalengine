#pragma once

#include <vector>

#include "Util/Vector.h"

namespace whal {

namespace ecs {
class Entity;
}

struct Animator;
struct Animation;

using AnimLogic = bool (*)(Animator& anim, ecs::Entity entity);

struct Animator {
    Vector2i currentPositionTexels = {0, 0};
    f32 fps = 4.0;
    std::vector<Animation> animations;
    // TODO design decision: require animation frames to be adjacent to each other? Base everything off of a root position + offsets?
    // should be based on tool i use to stitch textures

    // a "brain" function takes an animator + entity and chooses when to change frames
    AnimLogic* brain = nullptr;
};

struct Animation {
    const char* name;
    const s32 nFrames;
    Vector2i rootAtlasPositionPixels;
    Vector2i dimensionsPixels;
};

}  // namespace whal
