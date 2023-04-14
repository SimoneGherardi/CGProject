#include "game_engine.h"
#include "Models.h"
#include "math.h"
#include <glm/gtx/transform.hpp>
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"

flecs::entity DEBUGGO;

GameEngine::GameEngine(): _Camera(CameraInfos(1600, 900, 60, glm::vec3(0.4f, 1.2f, -10.0f)))
{
    //SetupPhysicsLogger();
    PhysicsWorld = PhysicsCommon.createPhysicsWorld();
    _PreviousFrameTime = std::chrono::system_clock::now();

    ECSWorld.import<Movement>();
    ECSWorld.import<Physics>();
    ECSWorld.import<Rendering>();

    RaycastTargets = ECSWorld.filter_builder()
        .term<Collider>()
        .term<RigidBody>().optional().oper(flecs::Or)
        .term<CollisionBody>().optional()
        .build();

    _TestEcs();
}

GameEngine& GameEngine::GetInstance()
{
    static GameEngine instance;
    return instance;
}

CameraInfos& GameEngine::Camera()
{
	return _Camera;
}

void GameEngine::_TestEcs()
{
    ECSWorld.entity("Right")
        .set<Transform>({ {8, 4, 0} })
        .set<RigidBody>({ 10.0f, rp3d::BodyType::DYNAMIC, NULL })
        .set<Collider>({ {1, 1, 1}, rp3d::CollisionShapeName::BOX, NULL })
        .set<Velocity>({ {0, 0, 0}, 1 })
        .set<Renderer>({ Models::SUZANNE });
    ECSWorld.entity("Left")
        .set<Transform>({ {-8, 4, 0} })
        .set<RigidBody>({ 10.0f, rp3d::BodyType::DYNAMIC, NULL })
        .set<Collider>({ {1, 1, 1}, rp3d::CollisionShapeName::BOX, NULL })
        .set<Velocity>({ {1, 0, 0}, 1 });
    ECSWorld.entity("Plane")
        .set<Transform>({ {0, 0, 0} })
        .set<RigidBody>({ 0, rp3d::BodyType::STATIC, NULL })
        .set<Collider>({ {100, 1, 100}, rp3d::CollisionShapeName::BOX, NULL });
    DEBUGGO = ECSWorld.entity("Debuggo")
        .set<Transform>({ {10, 10, 1} })
        .set<Renderer>({ Models::DEBUG });
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

void GameEngine::Loop(float delta)
{
    _CurrentFrameTime = std::chrono::system_clock::now();
    DeltaTime = _CurrentFrameTime - _PreviousFrameTime;
    _PreviousFrameTime = _CurrentFrameTime;
    Accumulator += DeltaTime;

    ECSWorld.progress();
}

rp3d::Vector3 GameEngine::WorldToCameraSpace(rp3d::Vector3 position)
{
    GameEngine &engine = GameEngine::GetInstance();
    glm::vec4 positionFromCamera = engine._Camera.ViewMatrix() * glm::vec4(position.x, position.y, position.z, 1);
    positionFromCamera = positionFromCamera / positionFromCamera.w;
    return rp3d::Vector3(positionFromCamera.x, positionFromCamera.y, positionFromCamera.z);
}

rp3d::Vector3 GameEngine::CameraToWorldSpace(rp3d::Vector3 position)
{
    GameEngine& engine = GameEngine::GetInstance();
    glm::vec4 positionInWorld = glm::inverse(engine._Camera.ViewMatrix()) * glm::vec4(position.x, position.y, position.z, 1);
    positionInWorld = positionInWorld / positionInWorld.w;
    return rp3d::Vector3(positionInWorld.x, positionInWorld.y, positionInWorld.z);
}

glm::vec3 GameEngine::WorldToScreenSpace(rp3d::Vector3 position)
{
    GameEngine &engine = GameEngine::GetInstance();
    auto x = glm::vec4(position.x, position.y, position.z, 1);
	glm::vec4 positionFromScreen = engine._Camera.Matrix() * x;
	positionFromScreen = positionFromScreen / positionFromScreen.w;
    return glm::vec3(positionFromScreen.x, positionFromScreen.y, positionFromScreen.z);
}

rp3d::Vector3 GameEngine::ScreenToWorldSpace(glm::vec3 screenPoint)
{
    GameEngine& engine = GameEngine::GetInstance();
    auto I = glm::inverse(engine._Camera.Matrix());
    auto d = I * glm::vec4(screenPoint.x, screenPoint.y, screenPoint.z, 1.0);
    d /= d.w;
    return rp3d::Vector3(d.x, d.y, d.z);
}

rp3d::decimal GatherAllRaycastCallback::notifyRaycastHit(const rp3d::RaycastInfo& info)
{
    RaycastInfo* raycastInfo = new RaycastInfo();
    raycastInfo->body = info.body;
    raycastInfo->worldPoint = info.worldPoint;
    raycastInfo->worldNormal = info.worldNormal;
    raycastInfo->hitFraction = info.hitFraction;
    raycastInfo->collider = info.collider;
    raycastInfo->meshSubpart = info.meshSubpart;
    raycastInfo->triangleIndex = info.triangleIndex;

    GameEngine &engine = GameEngine::GetInstance();

    engine.RaycastTargets.iter([&](flecs::iter& it) {
        flecs::id vs_id = it.id(2);
        if (vs_id == engine.ECSWorld.id<CollisionBody>()) {
            auto cb = it.field<CollisionBody>(2);
            for (auto i : it) {
                if (cb->Body == info.body) raycastInfo->Entity = it.entity(i);
			}
        }
        else if (vs_id == engine.ECSWorld.id<RigidBody>()) {
            auto rb = it.field<RigidBody>(2);
            for (auto i : it) {
                if (rb->Body == info.body) raycastInfo->Entity = it.entity(i);
            }
        }
    });
    
    Infos.push_back(raycastInfo);
    return rp3d::decimal(1.0);
}

std::vector<RaycastInfo*> GameEngine::RaycastFromCamera(glm::vec2 screenPoint, rp3d::decimal maxDistance)
{
    GameEngine &engine = GameEngine::GetInstance();
    CameraInfos camera = engine._Camera;
    rp3d::Vector3 origin{ camera.Position.x, camera.Position.y, camera.Position.z };
    auto pos = engine.ScreenToWorldSpace(glm::vec3(screenPoint.x, screenPoint.y, 0.95));
    DEBUGGO.get_mut<Transform>()->Position = rp3d::Vector3(
        pos.x, pos.y, pos.z
    );

    ECSWorld.entity()
        .set<Transform>({ {pos.x, pos.y, pos.z} })
        .set<Renderer>({ Models::TEST_TEXTURE });

    rp3d::Vector3 direction = pos - origin;
    direction.normalize();
    rp3d::Vector3 end = origin + direction * maxDistance;
    rp3d::Ray ray{ origin, end };

    GatherAllRaycastCallback callback;
    PhysicsWorld->raycast(ray, &callback);

	return callback.Infos;
}