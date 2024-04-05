#include "InputHandler.h"

#include <GLFW/glfw3.h>
namespace whal {

Input::Input() {
    loadMappings();
}

void Input::set(InputType input) {
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
        mIsJumpUsed = false;
        break;

    case InputType::PAUSE:
        mIsPause = mIsPause != true;
        break;

    case InputType::DEBUG:
        mIsDebug = mIsDebug != true;
        break;
    }
}

void Input::reset(InputType input) {
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
        break;

    default:
        break;
    }
}

void Input::loadMappings() const {
    // TODO load from file

    KeyMap.clear();

    KeyMap.insert({GLFW_KEY_A, InputType::LEFT});
    KeyMap.insert({GLFW_KEY_D, InputType::RIGHT});
    KeyMap.insert({GLFW_KEY_W, InputType::UP});
    KeyMap.insert({GLFW_KEY_S, InputType::DOWN});
    KeyMap.insert({GLFW_KEY_SPACE, InputType::JUMP});
    KeyMap.insert({GLFW_KEY_ESCAPE, InputType::PAUSE});
    KeyMap.insert({GLFW_KEY_0, InputType::DEBUG});
}

void Input::useJump() {
    mIsJumpUsed = true;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    auto search = Input::KeyMap.find(key);
    if (search == Input::KeyMap.end()) {
        return;
    }
    InputType input = search->second;

    if (action == GLFW_PRESS) {
        Input::getInstance().set(input);
    } else if (action == GLFW_RELEASE) {
        Input::getInstance().reset(input);
    }
}
}  // namespace whal
