#pragma once
#include <flecs.h>
#include "movement.h"
#include "ModelId.h"

typedef struct Renderer
{
    ModelId Mesh;
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

