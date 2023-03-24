#include "game_engine.h"

GameEngine::GameEngine()
{
    //SetupPhysicsLogger();
    PhysicsWorld = PhysicsCommon.createPhysicsWorld();
    _PreviousFrameTime = std::chrono::system_clock::now();

    ECSWorld.import<Movement>();
    ECSWorld.import<Physics>();
    ECSWorld.import<Rendering>();

    _TestEcs();
}

GameEngine& GameEngine::GetInstance()
{
    static GameEngine instance;
    return instance;
}

void GameEngine::_TestEcs()
{
    /*ECSWorld.entity()
        .set<Transform>({ {0, 0, 0} })
        .set<Velocity>({ {1, 0.5, 0}, 1 })
        .set<AngularVelocity>({ rp3d::Quaternion::fromEulerAngles(1, 0, 1), 5 })
        .set<Renderer>({ 0 });*/
    /*ECSWorld.entity("Falling")
        .set<Transform>({ {0, 10, 0} })
        .set<RigidBody>({10.0f, rp3d::BodyType::DYNAMIC, NULL})
        .set<Collider>({ {1, 1, 1}, rp3d::CollisionShapeName::CAPSULE, NULL })
        .set<Renderer>({});
    ECSWorld.entity("Plane")
        .set<Transform>({ {0, 0, 0} })
        .set<RigidBody>({0, rp3d::BodyType::STATIC, NULL})
        .set<Collider>({ {10, 1, 10}, rp3d::CollisionShapeName::BOX, NULL });*/
    ECSWorld.entity("Right")
        .set<Transform>({ {8, 4, 0} })
        .set<RigidBody>({ 10.0f, rp3d::BodyType::DYNAMIC, NULL })
        .set<Collider>({ {1, 1, 1}, rp3d::CollisionShapeName::BOX, NULL })
        .set<Velocity>({ {-1, 0, 0}, 1 })
        .set<Renderer>({});
    ECSWorld.entity("Left")
        .set<Transform>({ {-8, 4, 0} })
        .set<RigidBody>({ 10.0f, rp3d::BodyType::DYNAMIC, NULL })
        .set<Collider>({ {1, 1, 1}, rp3d::CollisionShapeName::BOX, NULL })
        .set<Velocity>({ {1, 0, 0}, 1 });
    ECSWorld.entity("Plane")
        .set<Transform>({ {0, 0, 0} })
        .set<RigidBody>({ 0, rp3d::BodyType::STATIC, NULL })
        .set<Collider>({ {100, 1, 100}, rp3d::CollisionShapeName::BOX, NULL });
    /*ECSWorld.entity("WallLeft")
        .set<Transform>({ {-90, 0, 0} })
        .set<RigidBody>({ 0, rp3d::BodyType::STATIC, NULL })
        .set<Collider>({ {1, 100, 100}, rp3d::CollisionShapeName::BOX, NULL });
    ECSWorld.entity("WallRight")
        .set<Transform>({ {90, 0, 0} })
        .set<RigidBody>({ 0, rp3d::BodyType::STATIC, NULL })
        .set<Collider>({ {1, 100, 100}, rp3d::CollisionShapeName::BOX, NULL });*/
}

void GameEngine::_SetupPhysicsLogger()
{
    // Create the default logger 
    rp3d::DefaultLogger* logger = PhysicsCommon.createDefaultLogger();

    // Log level (warnings and errors) 
    rp3d::uint logLevel = static_cast<rp3d::uint>(static_cast<rp3d::uint>(rp3d::Logger::Level::Information) | static_cast<rp3d::uint>(rp3d::Logger::Level::Error));

    // Output the logs into an HTML file 
    logger->addFileDestination("rp3d_log_aaa.html", logLevel, rp3d::DefaultLogger::Format::HTML);

    // Output the logs into the standard output 
    logger->addStreamDestination(std::cout, logLevel, rp3d::DefaultLogger::Format::Text);

    // Set the logger 
    PhysicsCommon.setLogger(logger);
}

std::chrono::system_clock::time_point GameEngine::GetCurrentFrameTime()
{
    return this->_CurrentFrameTime;
}

void GameEngine::Loop()
{
    while (true)
    {
        _CurrentFrameTime = std::chrono::system_clock::now();
        DeltaTime = _CurrentFrameTime - _PreviousFrameTime;
        _PreviousFrameTime = _CurrentFrameTime;
        Accumulator += DeltaTime;

        ECSWorld.progress();
    }
}