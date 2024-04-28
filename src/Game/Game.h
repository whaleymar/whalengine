#pragma once

#include "Gfx/Window.h"
#include "Map/Level.h"

using namespace whal;

class Game {
public:
    Game() = default;
    Game(const Game& other) = delete;
    void operator=(const Game&) = delete;

    bool startup();
    void mainloop();
    void end();

    void loadScene(std::string_view name);

private:
    std::unique_ptr<Window> mWindow;
    Scene mActiveScene;
    bool mIsRunning;
};
