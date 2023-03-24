#include "rendering.h"
#include "reactphysics3d/reactphysics3d.h"

void GetTransform(flecs::entity e, Transform& transform, Renderer& renderer)
{
    flecs::entity current = e;
    rp3d::Transform t = transform.LocalTransform();

    while (current.has_relation(flecs::ChildOf))
    {
        flecs::entity parent = current.parent();
        t = parent.get<Transform>()->LocalTransform() * t;
        current = parent;
    }
    renderer.GlobalTransform = t;
}

void PrintInfos(flecs::entity e, Renderer renderer)
{
    std::cout << renderer.GlobalTransform.to_string() << std::endl;
}

Rendering::Rendering(flecs::world& world)
{
    world.module<Rendering>("Rendering");

    _Renderer = world.component<Renderer>();

    _GetTransform = world.system<Transform, Renderer>()
        .kind(flecs::PreStore)
        .each(GetTransform);
    _CopyInfos = world.system<Renderer>()
        .kind(flecs::OnStore)
        .each(PrintInfos);
}
