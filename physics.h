#pragma once
#include <flecs.h>
#include <reactphysics3d/reactphysics3d.h>

typedef struct CollisionBody
{
    rp3d::CollisionBody* Body;
} CollisionBody;

typedef struct RigidBody
{
    float Weight;
    rp3d::BodyType Type;
    bool CanRotate = true;
    rp3d::RigidBody* Body;
} RigidBody;

typedef struct Collider
{
    rp3d::Vector3 Size;
    rp3d::CollisionShapeName Type;
    bool isTrigger = false;
    float bounciness = 0;
    rp3d::Collider* RP3DCollider;
} Collider;

class Physics
{
private:
    flecs::entity _CollisionBody;
    flecs::entity _RigidBody;
    flecs::entity _Collider;
    flecs::entity _CreateColliderForCollisionBody;
    flecs::entity _CreateColliderForRigidBody;
    flecs::entity _CreateCollisionBody;
    flecs::entity _CreateRigidBody;
    flecs::entity _CreateCollider;
    flecs::entity _TransformPositionToPhysicsPositionRigidbody;
    flecs::entity _PhysicsPositionToTransformPositionRigidbody;
    flecs::entity _TransformPositionToPhysicsPositionCollisionbody;
    flecs::entity _PhysicsPositionToTransformPositionCollisionbody;
    flecs::entity _UpdateWorld;
public:
    Physics(flecs::world& world);
};