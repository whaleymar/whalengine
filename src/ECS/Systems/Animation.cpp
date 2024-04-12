#include "Animation.h"

#include "ECS/Components/Animator.h"
#include "ECS/Components/Draw.h"
#include "ECS/Components/Position.h"
#include "ECS/Lib/ECS.h"
#include "Gfx/Texture.h"

namespace whal {

void AnimationSystem::update() {
    for (auto& [entityid, entity] : getEntities()) {
        auto& anim = entity.get<Animator>();
        if (anim.brain == nullptr) {
            continue;
        }
        auto& sprite = entity.get<Sprite>();

        if ((*anim.brain)(anim, entity)) {
            // frame changed
            const Frame frame = anim.getFrame();
            sprite.atlasPositionTexels = frame.atlasPositionTexels;
            sprite.isVertsUpdateNeeded = true;
        } else {
            // frame unchanged
            auto& pos = entity.get<Position>();
            if (pos.isDirectionChanged) {
                sprite.isVertsUpdateNeeded = true;
                pos.isDirectionChanged = false;
            }
        }
    }
}

}  // namespace whal
