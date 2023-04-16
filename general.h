#pragma once
#include <flecs.h>
#include "game_engine.h"

typedef struct Prefab
{
    PREFABS Prefab;
} Prefab;

class General
{
private:
    flecs::entity _Prefab;
public:
    General(flecs::world& world);
};