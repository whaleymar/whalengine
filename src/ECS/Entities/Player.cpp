#include <algorithm>

#include "ECS/Components/AnimUtil.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Draw.h"
#include "ECS/Components/PlayerControl.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Lib/ECS.h"
#include "ECS/Systems/Physics.h"
#include "Systems/System.h"
#include "Util/MathUtil.h"

#include "Gfx/GfxUtil.h"

namespace whal {

// RESEARCH this could be simplified. Ideally this should be read from a file generated by aseprite?
// File would look something like this:
// "basename","count", "secondsPerFile"
// "actor/player-run","4", "0.25"
// "actor/player-idle", "2", "1.0"
//
// this way, the only thing stored in code is animation names. Changing # of frames and file names is trivial
//
// per-frame duration could also be interesting, for a key-frame-like effect

namespace PlayerAnim {

enum AnimId : s32 { IDLE, RUN, JUMP, FALL };

static const AnimInfo S_PLAYER_ANIM_INFO = {
    {"actor/player-run", PlayerAnim::RUN, 4, 0.125},
    {"actor/player-idle", PlayerAnim::IDLE, 2, 1.0},
    {"actor/player-jump", PlayerAnim::JUMP, 1, 1.0},
    {"actor/player-fall", PlayerAnim::FALL, 1, 1.0},
};

// RESEARCH
// interesting concept to try:
// *transition animations*
// -> animations which play for a fixed # of cycles & knows which animation follows? Maybe a custom data structure is overkill & we only need a static
// nCyclesRemaining var in brain?
//
// struct TransitionAnimation : Animation {
//     s32 nCyclesMax = 1;
//     s32 nextAnimIx = -1;
// };
//
bool brain(Animator& animator, ecs::Entity entity) {
    auto& rb = entity.get<RigidBody>();
    auto& vel = entity.get<Velocity>();
    auto& sprite = entity.get<Sprite>();

    f32 unsquishStep = System::dt() * 1.50;
    sprite.scale = {approach(sprite.scale.x(), 1.0, unsquishStep), approach(sprite.scale.y(), 1.0, unsquishStep)};
    sprite.isVertsUpdateNeeded = true;
    if (rb.isGrounded) {
        if (rb.isLanding) {
            f32 squish = std::min(abs(vel.total.y()) / abs(TERMINAL_VELOCITY_Y), 1.0f);
            sprite.scale = {lerp(1, 1.25, squish), lerp(1, 0.8, squish)};
        }
        if (vel.total.x() != 0) {
            if (animator.setAnimation(RUN)) {
                return true;
            }
        } else {
            if (animator.setAnimation(IDLE)) {
                return true;
            }
        }
    } else {
        if (rb.isJumping) {
            if (animator.setAnimation(JUMP)) {
                sprite.scale = {0.8, 1.25};
                return true;
            }
        } else {
            if (animator.setAnimation(FALL)) {
                return true;
            }
        }
    }

    // animation did not change
    return basicAnimation(animator, entity);
}

}  // namespace PlayerAnim

Expected<ecs::Entity> createPlayer() {
    auto& ecs = ecs::ECS::getInstance();

    auto expected = ecs.entity();
    if (!expected.isExpected()) {
        return expected;
    }
    auto player = expected.value();
    Transform transform = Transform::tiles(15, 10);
    player.add(transform);
    player.add<Velocity>();
    player.add<PlayerControlRB>();
    // player.add<PlayerControlFree>();

    // graphics
    Animator animator;
    loadAnimations(animator, PlayerAnim::S_PLAYER_ANIM_INFO);
    animator.brain = &PlayerAnim::brain;
    player.add(animator);

    Sprite sprite = Sprite(Depth::Player, animator.getFrame());
    player.add(sprite);

    constexpr s32 width = 16;
    // constexpr s32 height = 16;
    constexpr s32 halfLenX = PIXELS_PER_TEXEL * width / 4;
    constexpr s32 halfLenY = PIXELS_PER_TEXEL * 6;
    player.add(ActorCollider(toFloatVec(transform.position) + Vector2f(0, halfLenY), Vector2i(halfLenX, halfLenY)));
    player.add<RigidBody>();

    return player;
}

}  // namespace whal
