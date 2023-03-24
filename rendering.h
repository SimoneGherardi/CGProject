#pragma once
#include <flecs.h>
#include "movement.h"

typedef struct Renderer
{
    int Mesh;
    rp3d::Transform GlobalTransform;
} Renderer;

class Rendering
{
private:
    flecs::entity _Renderer;
    flecs::entity _GetTransform;
    flecs::entity _CopyInfos;
public:
    Rendering(flecs::world& world);
};

