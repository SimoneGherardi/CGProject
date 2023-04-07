#include "game_engine.h"
#include "Models.h"
#include "math.h"
#include <glm/gtx/transform.hpp>
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"

flecs::entity DEBUGGO;

GameEngine::GameEngine(): _Camera(CameraInfos(1600, 900, 110, glm::vec3(0.4f, 1.2f, -10.0f)))
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
        .set<Velocity>({ {-1, 0, 0}, 1 })
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

glm::vec3 GameEngine::ScreenToWorldSpace(glm::vec2 screenPoint)
{
    /*
    * Il mapping da punto 2D a 3D restituisce una direzione.
    * In particolare è necessario procedere in 2 fasi, prima
    * de-proiettando il punto in 2D in modo da ottenere la direzione
    * in 3D (forzare .w=0 e normalizzare).
    * Questa direzione però non tiene conto della rotazione
    * della camera; pertanto è necessario moltiplicare la direzione
    * per l'inverso della view matrix.
    * (nota: visto che .w=0 la direzione non dipende dalla traslazione)
    */
    GameEngine& engine = GameEngine::GetInstance();
    auto V = engine._Camera.ViewMatrix();
    auto P = engine._Camera.ProjectionMatrix();
    auto iV = glm::inverse(V);
    auto iP = glm::inverse(P);
    // DO NOT CHANGE 177013.950320 ***IT IS EXTREMELY IMPORTANT***
    auto d0 = iP * glm::vec4(screenPoint.x, screenPoint.y, 177013.950320, 1.0);
    d0.w = 0;
    d0 = glm::normalize(d0);
    auto d = iV * d0;
    d = glm::normalize(d); // prob inutile
    return glm::vec3(d.x, d.y, d.z);
}

rp3d::decimal GatherAllRaycastCallback::notifyRaycastHit(const rp3d::RaycastInfo& info)
{
    rp3d::RaycastInfo* raycastInfo = new rp3d::RaycastInfo();
    raycastInfo->body = info.body;
    raycastInfo->worldPoint = info.worldPoint;
    raycastInfo->worldNormal = info.worldNormal;
    raycastInfo->hitFraction = info.hitFraction;
    raycastInfo->collider = info.collider;
    raycastInfo->meshSubpart = info.meshSubpart;
    raycastInfo->triangleIndex = info.triangleIndex;
    
    Infos.push_back(raycastInfo);
    return rp3d::decimal(1.0);
}

std::vector<rp3d::RaycastInfo*> GameEngine::RaycastFromCamera(glm::vec2 screenPoint, rp3d::decimal maxDistance)
{
    GameEngine &engine = GameEngine::GetInstance();
    CameraInfos camera = engine._Camera;
    rp3d::Vector3 origin{ camera.Position.x, camera.Position.y, camera.Position.z };
    auto dir = engine.ScreenToWorldSpace(screenPoint);
    auto camPos = glm::vec3(origin.x, origin.y, origin.z);
    auto pos = camPos + dir * maxDistance;
    std::cout << "distance from camera: " << glm::length(camPos - pos) << std::endl;
    DEBUGGO.get_mut<Transform>()->Position = rp3d::Vector3(
        pos.x, pos.y, pos.z
    );

    // rp3d::Vector3 screenPointInWorld = engine.ScreenToWorldSpace(screenPoint);
    rp3d::Vector3 screenPointInWorld = { 0,0,0 };
    rp3d::Vector3 direction = screenPointInWorld - origin;
    direction.normalize();
    rp3d::Vector3 end = origin + direction * maxDistance;
    rp3d::Ray ray{ origin, end };

    GatherAllRaycastCallback callback;
    PhysicsWorld->raycast(ray, &callback);

	return callback.Infos;
}