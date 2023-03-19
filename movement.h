#pragma once
#include <flecs.h>
#include "reactphysics3d/reactphysics3d.h"

typedef struct Transform
{
public:
    Transform* Parent;

    rp3d::Vector3 GetPosition();
    void SetPosition(rp3d::Vector3);
    rp3d::Quaternion GetRotation();
    void SetRotation(rp3d::Quaternion);
    rp3d::Transform GetGlobalTransform();
private:
    rp3d::Vector3 _Position = {0, 0, 0};
    rp3d::Quaternion _Rotation = rp3d::Quaternion::identity();
    rp3d::Transform _LocalTransform;
    rp3d::Transform _GlobalTransform;
    std::chrono::system_clock::time_point _LastGlobalTransformUpdate = std::chrono::system_clock::time_point::min();

    void _UpdateTransform();
} Transform;

typedef struct Velocity
{
    rp3d::Vector3 Direction = { 1, 0, 0 };
    float Magnitude = 0;
} Velocity;

typedef struct AngularVelocity
{
    rp3d::Quaternion Direction = rp3d::Quaternion::fromEulerAngles(0, 0, 0);
    float Magnitude = 0;
} AngularVelocity;

class Movement
{
private:
    flecs::entity _Transform;
    flecs::entity _Velocity;
    flecs::entity _AngularVelocity;
    flecs::entity _Move;
    flecs::entity _Rotate;
public:
    Movement(flecs::world& world);
};