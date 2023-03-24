#pragma once
#include <flecs.h>
#include <reactphysics3d/reactphysics3d.h>

typedef struct CollisionBody
{
    rp3d::CollisionBody* Body;
    // CollisionBody() : Body(NULL) {}
} CollisionBody;

typedef struct RigidBody
{
    float Weight;
    rp3d::BodyType Type;
    rp3d::RigidBody* Body;
    // RigidBody() : Body(NULL), Weight(70.f), Type(rp3d::BodyType::DYNAMIC) {}
} RigidBody;

typedef struct Collider
{
    rp3d::Vector3 Size;
    rp3d::CollisionShapeName Type;
    rp3d::Collider* RP3DCollider;
    // Collider() : RP3DCollider(NULL), Size({1, 1, 1}), Type(rp3d::CollisionShapeName::CAPSULE) {}
} Collider;

class Physics
{
private:
    flecs::entity _CollisionBody;
    flecs::entity _RigidBody;
    flecs::entity _Collider;
    flecs::entity _CreateColliderForCollisionBody;
    flecs::entity _CreateColliderForRigidBody;
    flecs::entity _CreateRigidBody;
    flecs::entity _CreateCollider;
    flecs::entity _TransformPositionToPhysicsPosition;
    flecs::entity _UpdateWorld;
    flecs::entity _PhysicsPositionToTransformPosition;
public:
    Physics(flecs::world& world);
};