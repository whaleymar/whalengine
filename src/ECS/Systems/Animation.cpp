#include "Animation.h"

#include "ECS/Components/Animator.h"
#include "ECS/Components/Draw.h"
#include "ECS/Lib/ECS.h"
#include "Gfx/Texture.h"

namespace whal {

void AnimationSystem::update() {
    for (auto& [entityid, entity] : getEntities()) {
        auto& anim = entity.get<Animator>();
        if (anim.brain == nullptr) {
            continue;
        }
        auto& draw = entity.get<Sprite>();

        if ((*anim.brain)(anim, entity)) {
            const Frame frame = anim.getFrame();
            draw.atlasPositionTexels = frame.atlasPositionTexels;
            draw.isVertsUpdateNeeded = true;
        }
    }
}

}  // namespace whal
