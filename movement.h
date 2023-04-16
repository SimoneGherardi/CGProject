#pragma once
#include <flecs.h>
#include "reactphysics3d/reactphysics3d.h"

typedef struct Transform
{
    rp3d::Vector3 Position = { 0, 0, 0 };
    rp3d::Quaternion Rotation = rp3d::Quaternion::identity();
    const rp3d::Transform LocalTransform() const;
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