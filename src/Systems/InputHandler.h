#pragma once

#include <SDL2/SDL.h>
#include <unordered_map>

namespace whal {

struct System;

enum class InputType { LEFT, RIGHT, UP, DOWN, JUMP, PAUSE, QUIT, DEBUG, GROWX, GROWY, SHRINKX, SHRINKY, MUSICTEST, RELOADSCENE };

void pollEvents();
void keyCallback(SDL_Event& event);

class InputHandler {
public:
    friend System;

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
    bool isQuit() const { return mIsQuit; }

    bool isJumpAvailable() const { return mIsJumpPressed; }

#ifndef NDEBUG
    bool isDebug() const { return mIsDebug; }
    bool isShrinkX() const { return mIsShrinkX; }
    bool isShrinkY() const { return mIsShrinkY; }
    bool isGrowX() const { return mIsGrowX; }
    bool isGrowY() const { return mIsGrowY; }
    bool isMusicDebug() const { return mIsMusicTest; }
    bool isReloadScene() const { return mIsReloadScene; }
#endif

    inline static std::unordered_map<int, InputType> KeyMap;

private:
    InputHandler();
    InputHandler(const InputHandler&) = delete;
    void operator=(const InputHandler&) = delete;

    bool mIsLeft = false;
    bool mIsRight = false;
    bool mIsUp = false;
    bool mIsDown = false;
    bool mIsJump = false;
    bool mIsPause = false;
    bool mIsQuit = false;

    bool mIsJumpPressed = false;

#ifndef NDEBUG
    bool mIsDebug = false;
    bool mIsMusicTest = false;
    bool mIsReloadScene = false;
    bool mIsShrinkX = false;
    bool mIsShrinkY = false;
    bool mIsGrowX = false;
    bool mIsGrowY = false;
#endif
};

}  // namespace whal
