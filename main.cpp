#include <flecs.h>
#include "test.h"

#include <reactphysics3d/reactphysics3d.h>
#include <flecs.h>
#include "vulkan.h"


using namespace reactphysics3d;

int testPhysics()
{
    PhysicsCommon physicsCommon;
    
    PhysicsWorld* world = physicsCommon.createPhysicsWorld();

    Vector3 position(0, 20, 0);
    Quaternion orientation = Quaternion::identity();
    Transform transform(position, orientation);
    RigidBody* body = world->createRigidBody(transform);

    const decimal timeStep = 1.0f / 60.0f;

    for (int i = 0; i < 20; i++) {

        world->update(timeStep);

        const Transform& transform = body->getTransform();
        const Vector3& position = transform.getPosition();

        std::cout << "Body Position: (" << position.x << ", " <<
            position.y << ", " << position.z << ")" << std::endl;
    }

    return 0;
}

void testECS()
{
    flecs::world world;
    world.import<Test>();

    auto a = world.entity()
        .set<Counter>({ 0 });
    auto b = world.entity()
        .set<Counter>({ 10 });
    auto c = world.entity()
        .set<Counter>({ 20 });
    world.system<Counter>().kind(flecs::OnUpdate).iter([](flecs::iter it) {
        printf("AAA\n");
    });

    world.progress();
    world.progress();
    world.progress();
    world.progress();
    world.progress();
}

int main()
{
    testPhysics();
    
    CGProject app;

    testECS();

    try
    {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}