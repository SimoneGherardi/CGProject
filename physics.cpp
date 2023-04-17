#include "flecs.h"
#include "physics.h"
#include "movement.h"
#include "game_engine.h"

void CreateCollisionBody(flecs::entity e, Transform& transform, CollisionBody& collisionBody)
{
    if (collisionBody.Body != NULL) return;

    GameEngine& engine = GameEngine::GetInstance();
    rp3d::Vector3 position(transform.Position.x, transform.Position.y, transform.Position.z);
    rp3d::Quaternion rotation(transform.Rotation.x, transform.Rotation.y, transform.Rotation.z, transform.Rotation.w);
    rp3d::Transform initialTransform(position, rotation);
    collisionBody.Body = engine.PhysicsWorld->createCollisionBody(initialTransform);
}

void CreateRigidBody(flecs::entity e, Transform& transform, RigidBody& rigidbody)
{
    if (rigidbody.Body != NULL) return;

    GameEngine& engine = GameEngine::GetInstance();
    rp3d::Vector3 position(transform.Position.x, transform.Position.y, transform.Position.z);
    rp3d::Quaternion rotation(transform.Rotation.x, transform.Rotation.y, transform.Rotation.z, transform.Rotation.w);
    rp3d::Transform initialTransform(position, rotation);
    rigidbody.Body = engine.PhysicsWorld->createRigidBody(initialTransform);
    rigidbody.Body->setType(rigidbody.Type);
    rigidbody.Body->setMass(rigidbody.Weight);
}

void CreateCollider(rp3d::CollisionBody* body, Collider& collider)
{
    if (body == NULL) return;
    if (collider.RP3DCollider != NULL) return;

    GameEngine& engine = GameEngine::GetInstance();
    rp3d::CollisionShape* shape;
    switch (collider.Type)
    {
    case rp3d::CollisionShapeName::CAPSULE:
        shape = engine.PhysicsCommon.createCapsuleShape(collider.Size.x / 2, collider.Size.y);
        break;
    case rp3d::CollisionShapeName::BOX:
        shape = engine.PhysicsCommon.createBoxShape(collider.Size / 2);
        break;
    case rp3d::CollisionShapeName::SPHERE:
    default:
        shape = engine.PhysicsCommon.createSphereShape(collider.Size.x / 2);
        break;
    }
    collider.RP3DCollider = body->addCollider(shape, rp3d::Transform::identity());
    collider.RP3DCollider->setIsTrigger(collider.isTrigger);
    collider.RP3DCollider->getMaterial().setBounciness(collider.bounciness);
}

void CreateColliderCollisioniBody(flecs::entity e, CollisionBody& body, Collider& collider)
{
    CreateCollider(body.Body, collider);
}

void CreateColliderRigidBody(flecs::entity e, RigidBody& body, Collider& collider)
{
    CreateCollider(body.Body, collider);
}

void TransformPositionToPhysicsPosition(flecs::entity e, Transform& transform, rp3d::CollisionBody* body)
{
    if (body == nullptr) return;
    body->setTransform(transform.LocalTransform());
}

void TransformPositionToPhysicsPositionRigidBody(flecs::entity e, Transform& transform, RigidBody& rigidbody)
{
    TransformPositionToPhysicsPosition(e, transform, rigidbody.Body);
}

void TransformPositionToPhysicsPositionCollisionBody(flecs::entity e, Transform& transform, CollisionBody& collisionbody)
{
    TransformPositionToPhysicsPosition(e, transform, collisionbody.Body);
}

void UpdatePhysicalWorld(flecs::iter it)
{
    GameEngine& engine = GameEngine::GetInstance();
    if (!engine.IsPhysicsActive) return;
    while (engine.Accumulator >= PHYSICS_TIMESTEP)
    {
        engine.PhysicsWorld->update(PHYSICS_TIMESTEP.count());
        engine.Accumulator -= PHYSICS_TIMESTEP;
    }
}

void PhysicsPositionToTransformPosition(flecs::entity e, Transform& transform, rp3d::CollisionBody* body)
{
    GameEngine& engine = GameEngine::GetInstance();
    if (!engine.IsPhysicsActive) return;
    if (body == NULL) return;
    rp3d::Transform physicsTransform = body->getTransform();
    transform.Position = physicsTransform.getPosition();
    transform.Rotation = physicsTransform.getOrientation();
}

void PhysicsPositionToTransformPositionRigidbody(flecs::entity e, Transform& transform, RigidBody& rigidbody)
{
    PhysicsPositionToTransformPosition(e, transform, rigidbody.Body);
}

void PhysicsPositionToTransformPositionCollisionbody(flecs::entity e, Transform& transform, CollisionBody& collisionbody)
{
    PhysicsPositionToTransformPosition(e, transform, collisionbody.Body);
}

Physics::Physics(flecs::world& world)
{
    world.module<Physics>("Physics");

    _CollisionBody = world.component<CollisionBody>();
    _RigidBody = world.component<RigidBody>();
    _Collider = world.component<Collider>();

    _CreateCollisionBody = world.system<Transform, CollisionBody>()
        .each(CreateCollisionBody);
    _CreateRigidBody = world.system<Transform, RigidBody>()
        .each(CreateRigidBody);
    _CreateColliderForCollisionBody = world.system<CollisionBody, Collider>()
        .each(CreateColliderCollisioniBody);
    _CreateColliderForRigidBody = world.system<RigidBody, Collider>()
        .each(CreateColliderRigidBody);
    _TransformPositionToPhysicsPositionRigidbody = world.system<Transform, RigidBody>()
        .kind(flecs::PostUpdate)
        .each(TransformPositionToPhysicsPositionRigidBody);
    _TransformPositionToPhysicsPositionCollisionbody = world.system<Transform, CollisionBody>()
        .kind(flecs::PostUpdate)
        .each(TransformPositionToPhysicsPositionCollisionBody);
    _UpdateWorld = world.system<RigidBody>()
        .kind(flecs::PostUpdate)
        .iter(UpdatePhysicalWorld);
    _PhysicsPositionToTransformPositionRigidbody = world.system<Transform, RigidBody>()
        .kind(flecs::PostUpdate)
        .each(PhysicsPositionToTransformPositionRigidbody);
    _PhysicsPositionToTransformPositionCollisionbody = world.system<Transform, CollisionBody>()
        .kind(flecs::PostUpdate)
        .each(PhysicsPositionToTransformPositionCollisionbody);
}