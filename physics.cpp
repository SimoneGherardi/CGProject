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

void CreateCollider(flecs::entity e, CollisionBody& body, Collider& collider)
{
    if (collider.Collider != NULL) return;

    GameEngine& engine = GameEngine::GetInstance();
    rp3d::CollisionShape* shape;
    switch (collider.Type)
    {
    case rp3d::CollisionShapeName::CAPSULE:
        shape = engine.PhysicsCommon.createCapsuleShape(c->size.x, c->size.y);
        break;
    case rp3d::CollisionShapeName::BOX:
        shape = engine.PhysicsCommon.createBoxShape(c->size);
        break;
    case rp3d::CollisionShapeName::SPHERE:
    default:
        shape = engine.PhysicsCommon.createSphereShape(c->size.x);
        break;
    }
    collider.Collider = body.Body->addCollider(shape, rp3d::Transform::identity());
}

void PhysicsCopyPosition(flecs::entity e, Transform& transform, RigidBody& rigidbody)
{
    if (rigidbody.Body == NULL) return;
    GameEngine& engine = GameEngine::GetInstance();
    rp3d::Transform physicsTransform = rigidbody.Body->getTransform();
    float factor = engine.Accumulator / PHYSIC_TIMESTEP;
    rp3d::Transform currentTransform = rp3d::Transform(transform.Position, transform.Rotation);
    rp3d::Transform interpolatedTransform = rp3d::Transform::interpolateTransforms(currentTransform, physicsTransform, factor);
    transform.Position = interpolatedTransform.getPosition();
    transform.Rotation = interpolatedTransform.getOrientation();
}

Physics::Physics(flecs::world& world)
{
    world.module<Physics>("Physics");

    _CollisionBody = world.component<CollisionBody>();
    _RigidBody = world.component<RigidBody>();
    _Collider = world.component<Collider>();

    _CreateRigidBody = world.system<Transform, RigidBody>()
        .each(PhysicsCopyPosition);
    _CreateCollider = world.system<CollisionBody, Collider>()
        .each(CreateCollider);
    _PhysicsCopyPosition = world.system<Transform, RigidBody>()
        .kind(flecs::PostUpdate)
        .each(PhysicsCopyPosition);
}