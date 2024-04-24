#pragma once

// based on https://github.com/Gaetz/SDL-OpenGL

#include <SDL2/SDL.h>
#include <memory>

#include "Util/Types.h"

namespace whal {

struct SDLWindowDestroyer {
    void operator()(SDL_Window* window) const { SDL_DestroyWindow(window); }
};

class Window {
public:
    Window(const char* title);
    ~Window();
    Window() = delete;
    Window(const Window&) = delete;

    bool init(s32 xPos, s32 yPos, s32 width, s32 height, bool isFullScreen = false);
    void swapBuffers() const;
    void checkErrors() const;
    void setFocus() const;
    void updateFPS(f32 dt);

private:
    std::unique_ptr<SDL_Window, SDLWindowDestroyer> mWindow;
    SDL_GLContext mGLContext;
    const char* mTitle;

    f64 mPrevSeconds = 0;
    f64 mCurSeconds = 0;
    s32 mFrameCount = 0;
};

}  // namespace whal
