#include "movement.h"

#include "game_engine.h"

rp3d::Vector3 Transform::GetPosition()
{
    return this->_Position;
}
void Transform::SetPosition(rp3d::Vector3 position)
{
    this->_Position = position;
    this->_UpdateTransform();
}
rp3d::Quaternion Transform::GetRotation()
{
    return this->_Rotation;
}
void Transform::SetRotation(rp3d::Quaternion rotation)
{
    this->_Rotation = rotation;
    this->_UpdateTransform();
}
void Transform::_UpdateTransform()
{
    this->_LocalTransform = rp3d::Transform(this->_Position, this->_Rotation);
}
rp3d::Transform Transform::GetGlobalTransform()
{
    GameEngine& engine = GameEngine::GetInstance();
    if (this->_LastGlobalTransformUpdate < engine.GetCurrentFrameTime())
    {
        this->_GlobalTransform = this->Parent->GetGlobalTransform() * this->_LocalTransform;
        this->_LastGlobalTransformUpdate = engine.GetCurrentFrameTime();
    }
    return this->_GlobalTransform;
}

void Move(flecs::entity e, Transform& transform, Velocity& speed)
{
    transform.SetPosition(transform.GetPosition() + (speed.Direction * speed.Magnitude));
}

void Rotate(flecs::entity e, Transform& transform, AngularVelocity& speed)
{
    transform.SetRotation(transform.GetRotation() + (speed.Direction * speed.Magnitude));
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