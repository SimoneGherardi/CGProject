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
        shape = engine.PhysicsCommon.createCapsuleShape(collider.Size.x, collider.Size.y);
        break;
    case rp3d::CollisionShapeName::BOX:
        shape = engine.PhysicsCommon.createBoxShape(collider.Size);
        break;
    case rp3d::CollisionShapeName::SPHERE:
    default:
        shape = engine.PhysicsCommon.createSphereShape(collider.Size.x);
        break;
    }
    collider.RP3DCollider = body->addCollider(shape, rp3d::Transform::identity());
}

void CreateColliderCollisioniBody(flecs::entity e, CollisionBody& body, Collider& collider)
{
    CreateCollider(body.Body, collider);
}

void CreateColliderRigidBody(flecs::entity e, RigidBody& body, Collider& collider)
{
    CreateCollider(body.Body, collider);
}

void TransformPositionToPhysicsPosition(flecs::entity e, Transform& transform, RigidBody& rigidbody)
{
    if (rigidbody.Body == NULL) return;
    rigidbody.Body->setTransform(transform.LocalTransform());
}

void UpdatePhysicalWorld(flecs::iter it)
{
    GameEngine& engine = GameEngine::GetInstance();
    while (engine.Accumulator >= PHYSICS_TIMESTEP)
    {
        engine.PhysicsWorld->update(PHYSICS_TIMESTEP.count());
        engine.Accumulator -= PHYSICS_TIMESTEP;
    }
}

void PhysicsPositionToTransformPosition(flecs::entity e, Transform& transform, RigidBody& rigidbody)
{
    if (rigidbody.Body == NULL) return;
    GameEngine& engine = GameEngine::GetInstance();
    rp3d::Transform physicsTransform = rigidbody.Body->getTransform();
    /*float factor = engine.Accumulator / PHYSIC_TIMESTEP;
    rp3d::Transform currentTransform = rp3d::Transform(transform.Position, transform.Rotation);
    rp3d::Transform interpolatedTransform = rp3d::Transform::interpolateTransforms(currentTransform, physicsTransform, factor);*/
    transform.Position = physicsTransform.getPosition();
    transform.Rotation = physicsTransform.getOrientation();
}

Physics::Physics(flecs::world& world)
{
    world.module<Physics>("Physics");

    _CollisionBody = world.component<CollisionBody>();
    _RigidBody = world.component<RigidBody>();
    _Collider = world.component<Collider>();

    _CollisionBody = world.system<Transform, CollisionBody>()
        .each(CreateCollisionBody);
    _CreateRigidBody = world.system<Transform, RigidBody>()
        .each(CreateRigidBody);
    _CreateColliderForCollisionBody = world.system<CollisionBody, Collider>()
        .each(CreateColliderCollisioniBody);
    _CreateColliderForRigidBody = world.system<RigidBody, Collider>()
        .each(CreateColliderRigidBody);
    _TransformPositionToPhysicsPosition = world.system<Transform, RigidBody>()
        .kind(flecs::PostUpdate)
        .each(TransformPositionToPhysicsPosition);
    _UpdateWorld = world.system<RigidBody>()
        .kind(flecs::PostUpdate)
        .iter(UpdatePhysicalWorld);
    _PhysicsPositionToTransformPosition = world.system<Transform, RigidBody>()
        .kind(flecs::PostUpdate)
        .each(PhysicsPositionToTransformPosition);
}