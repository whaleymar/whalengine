#pragma once

#include <format>
#include <vector>

#include "ECS/Components/Animator.h"
#include "Gfx/GLResourceManager.h"
#include "Gfx/GfxUtil.h"
#include "Gfx/Texture.h"
#include "Util/Print.h"

namespace whal {

using AnimInfo = std::vector<std::tuple<const char*, s32>>;

void loadAnimations(Animator& animator, AnimInfo& animInfo) {
    auto& spriteTexture = GLResourceManager::getInstance().getTexture(TEXNAME_SPRITE);

    for (auto [animBaseName, count] : animInfo) {
        std::vector<Frame> frames;
        for (s32 i = 0; i < count; i++) {
            auto animName = std::format("{}{}", animBaseName, i + 1);

            auto frame = spriteTexture.getFrame(animName.c_str());
            if (frame) {
                frames.push_back(*frame);
            } else {
                print("Failed to load animation frame: ", animName);
            }
        }

        animator.animations.push_back(Animation(animBaseName, frames));
    }
}

}  // namespace whal
