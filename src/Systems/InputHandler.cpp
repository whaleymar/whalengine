#include "InputHandler.h"

// #include <GLFW/glfw3.h>

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

    case InputType::DEBUG:
        mIsDebug = mIsDebug != true;
        break;

#ifndef NDEBUG
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
#endif

    default:
        break;
    }
}

void InputHandler::loadMappings() const {
    // EVENTUALLY load from file once i have, like, menus working

    KeyMap.clear();

    //     KeyMap.insert({GLFW_KEY_A, InputType::LEFT});
    //     KeyMap.insert({GLFW_KEY_D, InputType::RIGHT});
    //     KeyMap.insert({GLFW_KEY_W, InputType::UP});
    //     KeyMap.insert({GLFW_KEY_S, InputType::DOWN});
    //     KeyMap.insert({GLFW_KEY_SPACE, InputType::JUMP});
    //     KeyMap.insert({GLFW_KEY_ESCAPE, InputType::PAUSE});
    //     KeyMap.insert({GLFW_KEY_0, InputType::DEBUG});
    //
    // #ifndef NDEBUG
    //     KeyMap.insert({GLFW_KEY_LEFT, InputType::SHRINKX});
    //     KeyMap.insert({GLFW_KEY_RIGHT, InputType::GROWX});
    //     KeyMap.insert({GLFW_KEY_DOWN, InputType::SHRINKY});
    //     KeyMap.insert({GLFW_KEY_UP, InputType::GROWY});
    // #endif
}

void InputHandler::useJump() {
    mIsJumpPressed = false;
}

// void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
//     auto search = InputHandler::KeyMap.find(key);
//     if (search == InputHandler::KeyMap.end()) {
//         return;
//     }
//     InputType input = search->second;
//
//     if (action == GLFW_PRESS) {
//         System::input.set(input);
//     } else if (action == GLFW_RELEASE) {
//         System::input.reset(input);
//     }
// }
}  // namespace whal
