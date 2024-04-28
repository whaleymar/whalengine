#include "Game/Game.h"

int main() {
    Game game;
    bool startupFailed = game.startup();
    if (startupFailed) {
        return -1;
    }

    game.mainloop();
    game.end();

    return 0;
}
