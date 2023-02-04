#include "movement.h"

void Move(flecs::entity e, Transform& transform, Velocity& speed)
{
    transform.position = transform.position + (speed.direction * speed.magnitude);
}

void Rotate(flecs::entity e, Transform& transform, AngularVelocity& speed)
{
    transform.rotation = transform.rotation + (speed.direction * speed.magnitude);
}

Movement::Movement(flecs::world& world)
{
    world.module<Movement>("Movement");

    _Transform = world.component<Transform>();
    _Velocity = world.component<Velocity>();
    _AngularVelocity = world.component<AngularVelocity>();

    _Move = world.system<Transform, Velocity>()
        .kind(flecs::PostUpdate)
        .each(Move);
    _Rotate = world.system<Transform, AngularVelocity>()
        .kind(flecs::PostUpdate)
        .each(Rotate);
}