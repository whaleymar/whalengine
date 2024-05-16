#include "EventListeners.h"

#include "ECS/Components/Draw.h"
#include "ECS/Components/Tags.h"
#include "ECS/Entities/Player.h"
#include "ECS/Lib/ECS.h"
#include "Systems/Audio.h"
#include "Systems/System.h"

namespace whal {

void startListeners() {
    System::eventMgr.registerListener(Event::DEATH_EVENT, Listeners::PLAYER_DEATH_LISTENER);
}

void killListeners() {
    System::eventMgr.stopListening(Event::DEATH_EVENT, Listeners::PLAYER_DEATH_LISTENER);
}

void onEntityDeath(ecs::Entity entity) {
    // if (entity.has<Name>()) {
    //     print("Killed entity: ", entity.get<Name>());
    // }
    if (!entity.has<Player>()) {
        return;
    }
    Sprite sprite;  // needs to be created in main thread bc OpenGL
    System::schedule.after(&createPlayerAsynch, 2, sprite);
    System::audio.play(Sfx::GAMEOVER);
}

}  // namespace whal
