#pragma once

#include "Events.h"

namespace whal {

namespace ecs {
class Entity;
}

void onPlayerDeath(ecs::Entity entity);

void startListeners();
void killListeners();

namespace Listeners {

inline auto PLAYER_DEATH_LISTENER = EventListener<ecs::Entity>(&onPlayerDeath);

}

}  // namespace whal
