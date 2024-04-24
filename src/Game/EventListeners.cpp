#include "EventListeners.h"

#include "ECS/Components/Tags.h"
#include "ECS/Lib/ECS.h"
#include "Systems/Audio.h"
#include "Systems/System.h"
#include "Util/Print.h"

namespace whal {

void startListeners() {
    System::eventMgr.registerListener(Event::DEATH_EVENT, Listeners::PLAYER_DEATH_LISTENER);
}

void killListeners() {
    System::eventMgr.stopListening(Event::DEATH_EVENT, Listeners::PLAYER_DEATH_LISTENER);
}

void onPlayerDeath(ecs::Entity entity) {
    if (!entity.has<Player>()) {
        return;
    }
    print("playing death sound");
    System::audio.play(Sfx::GAMEOVER);
}

}  // namespace whal
