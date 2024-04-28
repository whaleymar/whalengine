#include "Game/Game.h"

int main() {
    Game& game = Game::instance();

    bool startupFailed = game.startup();
    if (startupFailed) {
        return -1;
    }

    game.mainloop();
    game.end();

    return 0;
}
