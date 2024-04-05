#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>

namespace whal {

enum class InputType { LEFT, RIGHT, UP, DOWN, JUMP, PAUSE, DEBUG };

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// TODO track frame/time of input for buffering
class Input {
public:
    static Input& getInstance() {
        static Input instance;
        return instance;
    }

    void set(InputType input);
    void reset(InputType input);
    void loadMappings() const;
    void useJump();

    bool isLeft() const { return mIsLeft; }
    bool isRight() const { return mIsRight; }
    bool isUp() const { return mIsUp; }
    bool isDown() const { return mIsDown; }
    bool isJump() const { return mIsJump; }
    bool isPause() const { return mIsPause; }
    bool isDebug() const { return mIsDebug; }

    bool isJumpUsed() const { return mIsJumpUsed; }

    inline static std::unordered_map<int, InputType> KeyMap;

private:
    Input();
    Input(const Input&) = delete;
    void operator=(const Input&) = delete;

    bool mIsLeft = false;
    bool mIsRight = false;
    bool mIsUp = false;
    bool mIsDown = false;
    bool mIsJump = false;
    bool mIsPause = false;
    bool mIsDebug = false;

    bool mIsJumpUsed = false;
};

}  // namespace whal
