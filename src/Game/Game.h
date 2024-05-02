#pragma once

#include "Gfx/Window.h"
#include "Map/Level.h"
#include "Systems/Event.h"

using namespace whal;

class Game {
public:
    static Game& instance() {
        static Game instance_;
        return instance_;
    }

    Game(const Game& other) = delete;
    void operator=(const Game&) = delete;

    bool startup();
    void mainloop();
    void end();

    std::optional<Error> loadScene(const char* name);
    void unloadScene();
    std::optional<Error> reloadScene();
    Scene& getScene();
    void updateLoadedLevels(Vector2f cameraWorldPosPixels);
    void updateLevelCamera();

private:
    Game();
    std::unique_ptr<Window> mWindow;
    Scene mActiveScene;
    EventListener<ecs::Entity> mEntityDeathListener;
    bool mIsSceneLoaded;
};
