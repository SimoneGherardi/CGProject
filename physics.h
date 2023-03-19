#pragma once
#include <flecs.h>
#include <reactphysics3d/reactphysics3d.h>

typedef struct CollisionBody
{
    rp3d::CollisionBody* Body;
} CollisionBody;

typedef struct RigidBody: CollisionBody
{
    rp3d::RigidBody* Body;
    rp3d::BodyType Type = rp3d::BodyType::DYNAMIC;
    float Weight = 70;
} RigidBody;

typedef struct Collider
{
    rp3d::Collider* Collider;
    rp3d::CollisionShapeName Type = rp3d::CollisionShapeName::CAPSULE;
    rp3d::Vector3 Size = rp3d::Vector3(1, 1, 1);
} Collider;

class Physics
{
private:
    flecs::entity _CollisionBody;
    flecs::entity _RigidBody;
    flecs::entity _Collider;
    flecs::entity _CreateCollisionBody;
    flecs::entity _CreateRigidBody;
    flecs::entity _CreateCollider;
    flecs::entity _PhysicsCopyPosition;
public:
    Physics(flecs::world& world);
};