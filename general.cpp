#include "general.h"

General::General(flecs::world& world)
{
    world.module<General>("General");
    _Prefab = world.component<Prefab>();
}