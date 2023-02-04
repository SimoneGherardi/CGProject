#include "flecs.h"
#include "physics.h"
#include "movement.h"
#include "game_engine.h"

void CreateCollisionBody(flecs::entity e, Transform& transform, CollisionBody& collisionBody)
{
    if (collisionBody.body != NULL) return;

    GameEngine& engine = GameEngine::GetInstance();
    rp3d::Vector3 position(transform.position.x, transform.position.y, transform.position.z);
    rp3d::Quaternion rotation(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);
    rp3d::Transform initialTransform(position, rotation);
    collisionBody.body = engine.PhysicsWorld->createCollisionBody(initialTransform);
}

void CreateRigidBody(flecs::entity e, Transform& transform, RigidBody& rigidbody)
{
    if (rigidbody.body != NULL) return;

    GameEngine& engine = GameEngine::GetInstance();
    rp3d::Vector3 position(transform.position.x, transform.position.y, transform.position.z);
    rp3d::Quaternion rotation(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);
    rp3d::Transform initialTransform(position, rotation);
    rigidbody.body = engine.PhysicsWorld->createRigidBody(initialTransform);
    rigidbody.body->setType(rigidbody.type);
    rigidbody.body->setMass(rigidbody.weight);
}

void CreateCollider(flecs::entity e, CollisionBody& body, Collider& collider)
{
    if (collider.collider != NULL) return;

    GameEngine& engine = GameEngine::GetInstance();
    rp3d::CollisionShape* shape;
    switch (collider.type)
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
    collider.collider = body.body->addCollider(shape, rp3d::Transform::identity());
}

void PhysicsCopyPosition(flecs::entity e, Transform& transform, RigidBody& rigidbody)
{
    if (rigidbody.body == NULL) return;
    GameEngine& engine = GameEngine::GetInstance();
    rp3d::Transform physicsTransform = rigidbody.body->getTransform();
    float factor = engine.Accumulator / PHYSIC_TIMESTEP;
    rp3d::Transform currentTransform = rp3d::Transform(transform.position, transform.rotation);
    rp3d::Transform interpolatedTransform = rp3d::Transform::interpolateTransforms(currentTransform, physicsTransform, factor);
    transform.position = interpolatedTransform.getPosition();
    transform.rotation = interpolatedTransform.getOrientation();
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