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

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
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

    mWindow = std::unique_ptr<SDL_Window, SDLWindowDestroyer>(
        SDL_CreateWindow(mTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags));

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

}  // namespace whal
