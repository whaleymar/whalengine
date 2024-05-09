#include "Gfx/Window.h"

#include <glad/gl.h>

#include <SDL2/SDL.h>

#include "Util/Print.h"

namespace whal {

Window::Window(const char* title) : mTitle(title){};

Window::~Window() {
    if (mGLContext) {
        SDL_GL_DeleteContext(mGLContext);
    }
    SDL_Quit();
}

bool Window::init(s32 xPos, s32 yPos, s32 width, s32 height, bool isFullScreen) {
    s32 flags = SDL_WINDOW_OPENGL;
    if (isFullScreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        print("Failed to init SDL");
        print(SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    mWindow = std::unique_ptr<SDL_Window, SDLWindowDestroyer>(SDL_CreateWindow(mTitle, xPos, yPos, width, height, flags));

    if (!mWindow) {
        print("Failed to initialize SDL window");
        print(SDL_GetError());
        return false;
    }

    mGLContext = SDL_GL_CreateContext(mWindow.get());
    if (!mGLContext) {
        print("Failed to create OpenGL context");
        print(SDL_GetError());
        return false;
    }

    SDL_GL_LoadLibrary(NULL);
    int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
    if (version == 0) {
        print("Failed to initialize OpenGL context");
        print(SDL_GetError());
        return false;
    }
    print<Format({"", "\n"})>("Loaded OpenGL ", GLAD_VERSION_MAJOR(version), ".", GLAD_VERSION_MINOR(version));

    // glViewport(0, 0, WINDOW_WIDTH_PIXELS, WINDOW_HEIGHT_PIXELS); idk what this does
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);  // not working right
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void Window::swapBuffers() const {
    SDL_GL_SwapWindow(mWindow.get());
}

void Window::checkErrors() const {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        print(err);
    }
}

void Window::setFocus() const {
    SDL_SetWindowInputFocus(mWindow.get());
}

void Window::updateFPS(f32 fps) {
    char buf[128];
#if __linux__
    sprintf(buf, "%s @ fps: %.2f", mTitle, fps);
#else
    sprintf_s(buf, "%s @ fps: %.2f", mTitle, fps);
#endif
    SDL_SetWindowTitle(mWindow.get(), buf);
}

}  // namespace whal
