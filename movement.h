#pragma once
#include <flecs.h>
#include "reactphysics3d/reactphysics3d.h"

typedef struct Transform
{
    rp3d::Vector3 position = {0, 0, 0};
    rp3d::Quaternion rotation = rp3d::Quaternion::identity();
} Transform;

typedef struct Velocity
{
    rp3d::Vector3 direction = { 1, 0, 0 };
    float magnitude = 0;
} Velocity;

typedef struct AngularVelocity
{
    rp3d::Quaternion direction = rp3d::Quaternion::fromEulerAngles(0, 0, 0);
    float magnitude = 0;
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