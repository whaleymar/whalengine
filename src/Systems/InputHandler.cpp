#include "InputHandler.h"

#include <SDL2/SDL.h>

#include "System.h"

namespace whal {

InputHandler::InputHandler() {
    loadMappings();
}

void InputHandler::set(InputType input) {
    switch (input) {
    case InputType::LEFT:
        mIsLeft = true;
        break;

    case InputType::RIGHT:
        mIsRight = true;
        break;

    case InputType::UP:
        mIsUp = true;
        break;

    case InputType::DOWN:
        mIsDown = true;
        break;

    case InputType::JUMP:
        mIsJump = true;
        mIsJumpPressed = true;
        break;

    case InputType::PAUSE:
        mIsPause = mIsPause != true;
        break;

    case InputType::QUIT:
        mIsQuit = true;
        break;

#ifndef NDEBUG
    case InputType::DEBUG:
        mIsDebug = mIsDebug != true;
        break;

    case InputType::SHRINKX:
        mIsShrinkX = true;
        break;

    case InputType::SHRINKY:
        mIsShrinkY = true;
        break;

    case InputType::GROWX:
        mIsGrowX = true;
        break;

    case InputType::GROWY:
        mIsGrowY = true;
        break;

    case InputType::MUSICTEST:
        mIsMusicTest = true;
        break;

    case InputType::RELOADSCENE:
        mIsReloadScene = true;
        break;

    case InputType::TIMETEST:
        mIsTimeTest = true;
        break;
    case InputType::KILLPLAYER:
        mIsKillPlayer = true;
        break;
#endif
    }
}

void InputHandler::reset(InputType input) {
    switch (input) {
    case InputType::LEFT:
        mIsLeft = false;
        break;

    case InputType::RIGHT:
        mIsRight = false;
        break;

    case InputType::UP:
        mIsUp = false;
        break;

    case InputType::DOWN:
        mIsDown = false;
        break;

    case InputType::JUMP:
        mIsJump = false;
        mIsJumpPressed = false;
        break;

#ifndef NDEBUG
    case InputType::SHRINKX:
        mIsShrinkX = false;
        break;

    case InputType::SHRINKY:
        mIsShrinkY = false;
        break;

    case InputType::GROWX:
        mIsGrowX = false;
        break;

    case InputType::GROWY:
        mIsGrowY = false;
        break;

    case InputType::MUSICTEST:
        mIsMusicTest = false;
        break;

    case InputType::RELOADSCENE:
        mIsReloadScene = false;
        break;

    case InputType::TIMETEST:
        mIsTimeTest = false;
        break;
    case InputType::KILLPLAYER:
        mIsKillPlayer = false;
        break;
#endif

    default:
        break;
    }
}

void InputHandler::loadMappings() const {
    // EVENTUALLY load from file once i have, like, menus working

    KeyMap.clear();
    KeyMap.insert({SDLK_a, InputType::LEFT});
    KeyMap.insert({SDLK_d, InputType::RIGHT});
    KeyMap.insert({SDLK_w, InputType::UP});
    KeyMap.insert({SDLK_s, InputType::DOWN});
    KeyMap.insert({SDLK_SPACE, InputType::JUMP});
    KeyMap.insert({SDLK_ESCAPE, InputType::QUIT});

#ifndef NDEBUG
    KeyMap.insert({SDLK_0, InputType::DEBUG});
    KeyMap.insert({SDLK_m, InputType::MUSICTEST});
    KeyMap.insert({SDLK_r, InputType::RELOADSCENE});
    KeyMap.insert({SDLK_t, InputType::TIMETEST});
    KeyMap.insert({SDLK_k, InputType::KILLPLAYER});
//     KeyMap.insert({GLFW_KEY_LEFT, InputType::SHRINKX});
//     KeyMap.insert({GLFW_KEY_RIGHT, InputType::GROWX});
//     KeyMap.insert({GLFW_KEY_DOWN, InputType::SHRINKY});
//     KeyMap.insert({GLFW_KEY_UP, InputType::GROWY});
#endif
}

void InputHandler::useJump() {
    mIsJumpPressed = false;
}

void keyCallback(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN && event.key.repeat != 0) {
        return;
    }
    auto search = InputHandler::KeyMap.find(event.key.keysym.sym);
    if (search == InputHandler::KeyMap.end()) {
        return;
    }

    InputType input = search->second;
    if (event.type == SDL_KEYDOWN) {
        System::input.set(input);
    } else if (event.type == SDL_KEYUP) {
        System::input.reset(input);
    }
}

void pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            System::input.set(InputType::QUIT);
            break;
        }
        keyCallback(event);
    }
}

}  // namespace whal
