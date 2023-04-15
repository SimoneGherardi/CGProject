#include "movement.h"

#include "game_engine.h"

const rp3d::Transform Transform::LocalTransform() const
{
    return rp3d::Transform(this->Position, this->Rotation);
}

void Move(flecs::iter it, Transform* transform, Velocity* velocity)
{
    auto delta = GameEngine::GetInstance().DeltaTime;
    for (int i : it)
    {
        transform[i].Position = transform[i].Position + (velocity[i].Direction * velocity[i].Magnitude) * delta.count();
    }
}

void Rotate(flecs::iter it, Transform* transform, AngularVelocity* speed)
{
    auto delta = GameEngine::GetInstance().DeltaTime;
    for (int i : it)
    {
        transform[i].Rotation = transform[i].Rotation * speed[i].Direction * speed[i].Magnitude * delta.count();
        transform[i].Rotation.normalize();
    }
}

Movement::Movement(flecs::world& world)
{
    world.module<Movement>("Movement");

    _Transform = world.component<Transform>();
    _Velocity = world.component<Velocity>();
    _AngularVelocity = world.component<AngularVelocity>();

    _Move = world.system<Transform, Velocity>()
        .kind(flecs::OnUpdate)
        .iter(Move);
    _Rotate = world.system<Transform, AngularVelocity>()
        .kind(flecs::OnUpdate)
        .iter(Rotate);
}