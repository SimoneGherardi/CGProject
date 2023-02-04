#pragma once
#include <flecs.h>
#include <reactphysics3d/reactphysics3d.h>

typedef struct CollisionBody
{
    rp3d::CollisionBody* body;
} CollisionBody;

typedef struct RigidBody: CollisionBody
{
    rp3d::RigidBody* body;
    rp3d::BodyType type = rp3d::BodyType::DYNAMIC;
    float weight = 70;
} RigidBody;

typedef struct Collider
{
    rp3d::Collider* collider;
    rp3d::CollisionShapeName type = rp3d::CollisionShapeName::CAPSULE;
    rp3d::Vector3 size = rp3d::Vector3(1, 1, 1);
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