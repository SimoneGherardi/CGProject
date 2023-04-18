#include "game_engine.h"
#include "Models.h"
#include "math.h"
#include <glm/gtx/transform.hpp>
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <filesystem>
#include <sys/stat.h>

#if _WIN32
#else
#define sscanf_s(buf, fmt, ...) sscanf((buf), (fmt), __VA_ARGS__)
#endif

flecs::entity DEBUGGO;

GameEngine::GameEngine()
{
    SetIsEditor(IsEditor);

    //SetupPhysicsLogger();
    PhysicsWorld = PhysicsCommon.createPhysicsWorld();
    PhysicsWorld->setEventListener(new PhysicsEventListener());
    _PreviousFrameTime = std::chrono::system_clock::now();

    ECSWorld.import<General>();
    ECSWorld.import<Movement>();
    ECSWorld.import<Physics>();
    ECSWorld.import<Rendering>();

    RaycastTargets = ECSWorld.filter_builder()
        .term<Collider>()
        .term<RigidBody>().optional()
        .term<CollisionBody>().optional()
        .build();

    _InitPrefabs();
    
    ECSWorld.entity("skybox")
        .set<Transform>({ {0, 0, 0} })
        .set<Renderer>({Models::SKYBOX_MODEL_ID});

    /*DEBUGGO = ECSWorld.entity("Debuggo")
        .set<Transform>({ {10, 10, 1} })
        .set<Renderer>({ Models::DEBUG });*/
    //_TestEcs();
}

GameEngine& GameEngine::GetInstance()
{
    static GameEngine instance;
    return instance;
}

CameraInfos& GameEngine::Camera()
{
    if (_Camera == nullptr)
    {
        _Camera = new CameraInfos(60, glm::vec3(0, 7, 14));
    }
	return *_Camera;
}

void GameEngine::_InitPrefabs()
{
    _Prefabs[PREFABS::MONKEY] = [this](const char* name) {
        return ECSWorld.entity(name)
            .set<Prefab>({PREFABS::MONKEY})
            .add<Transform>()
            .set<RigidBody>({ 10.0f, rp3d::BodyType::DYNAMIC, true, NULL })
            .set<Collider>({ {2, 1, 1}, rp3d::CollisionShapeName::SPHERE, false, 0.5, NULL })
            .add<Velocity>()
            .set<Renderer>({ Models::SUZANNE });
    };
    _Prefabs[PREFABS::BUSH] = [this](const char* name) {
        return ECSWorld.entity(name)
            .set<Prefab>({ PREFABS::BUSH })
            .add<Transform>()
            .set<CollisionBody>({ NULL })
            .set<Collider>({ {1, 0.1, 1}, rp3d::CollisionShapeName::BOX, true, 0.5, NULL })
            .set<Renderer>({ Models::BUSH });
    };
    _Prefabs[PREFABS::COIN] = [this](const char* name) {
        return ECSWorld.entity(name)
            .set<Prefab>({ PREFABS::COIN })
            .add<Transform>()
            .set<RigidBody>({ 0, rp3d::BodyType::STATIC, true, NULL })
            .set<Collider>({ {0.2, 0.9, 0.9}, rp3d::CollisionShapeName::BOX, true, 0.5, NULL })
            .set<AngularVelocity>({ rp3d::Quaternion::fromEulerAngles(0, 0.001, 0), 1 })
            .set<Renderer>({ Models::COIN });
    };
    _Prefabs[PREFABS::GRASSBLOCK] = [this](const char* name) {
        return ECSWorld.entity(name)
            .set<Prefab>({ PREFABS::GRASSBLOCK })
            .add<Transform>()
            .set<RigidBody>({ 0, rp3d::BodyType::STATIC, true, NULL })
            .set<Collider>({ {2.1, 2.1, 2.1}, rp3d::CollisionShapeName::BOX, false, 0.1, NULL })
            .set<Renderer>({ Models::GRASSBLOCK });
    };
    _Prefabs[PREFABS::ROCK1] = [this](const char* name) {
        return ECSWorld.entity(name)
            .set<Prefab>({ PREFABS::ROCK1 })
            .add<Transform>()
            .set<RigidBody>({ 0, rp3d::BodyType::STATIC, true, NULL })
            .set<Collider>({ {2, 1, 1}, rp3d::CollisionShapeName::SPHERE, false, 0.5, NULL })
            .set<Renderer>({ Models::ROCK1 });
    };
    _Prefabs[PREFABS::ROCK2] = [this](const char* name) {
        return ECSWorld.entity(name)
            .set<Prefab>({ PREFABS::ROCK2 })
            .add<Transform>()
            .set<RigidBody>({ 0, rp3d::BodyType::STATIC, true, NULL })
            .set<Collider>({ {1.7, 1, 1}, rp3d::CollisionShapeName::SPHERE, false, 0.5, NULL })
            .set<Renderer>({ Models::ROCK2 });
    };
    _Prefabs[PREFABS::SIGN] = [this](const char* name) {
        return ECSWorld.entity(name)
            .set<Prefab>({ PREFABS::SIGN })
            .add<Transform>()
            .set<RigidBody>({ 0, rp3d::BodyType::STATIC, true, NULL })
            .set<Collider>({ {0.1, 1.8, 0.1}, rp3d::CollisionShapeName::BOX, false, 0.5, NULL })
            .set<Renderer>({ Models::SIGN });
    };
    _Prefabs[PREFABS::TREE1] = [this](const char* name) {
        return ECSWorld.entity(name)
            .set<Prefab>({ PREFABS::TREE1 })
            .add<Transform>()
            .set<RigidBody>({ 0, rp3d::BodyType::STATIC, true, NULL })
            .set<Collider>({ {0.4, 3.5, 0.2}, rp3d::CollisionShapeName::BOX, false, 0.5, NULL })
            .set<Renderer>({ Models::TREE1 });
    };
    _Prefabs[PREFABS::TREE2] = [this](const char* name) {
        return ECSWorld.entity(name)
            .set<Prefab>({ PREFABS::TREE2 })
            .add<Transform>()
            .set<RigidBody>({ 0, rp3d::BodyType::STATIC, true, NULL })
            .set<Collider>({ {0.4, 4.5, 0.2}, rp3d::CollisionShapeName::BOX, false, 0.5, NULL })
            .set<Renderer>({ Models::TREE2 });
    };
    _Prefabs[PREFABS::WOODBRIDGE] = [this](const char* name) {
        return ECSWorld.entity(name)
            .set<Prefab>({ PREFABS::WOODBRIDGE })
            .add<Transform>()
            .set<RigidBody>({ 0, rp3d::BodyType::STATIC, true, NULL })
            .set<Collider>({ {2, 0.2, 2}, rp3d::CollisionShapeName::BOX, false, 0.5, NULL })
            .set<Renderer>({ Models::WOODBRIDGE });
    };
    _Prefabs[PREFABS::WOODPLATFORM] = [this](const char* name) {
        return ECSWorld.entity(name)
            .set<Prefab>({ PREFABS::WOODPLATFORM })
            .add<Transform>()
            .set<RigidBody>({ 0, rp3d::BodyType::STATIC, true, NULL })
            .set<Collider>({ {1.5, 0.8, 1.5}, rp3d::CollisionShapeName::BOX, false, 0.5, NULL })
            .set<Renderer>({ Models::WOODPLATFORM });
    };
    _Prefabs[PREFABS::WOODSHELF] = [this](const char* name) {
        return ECSWorld.entity(name)
            .set<Prefab>({ PREFABS::WOODSHELF })
            .add<Transform>()
            .set<RigidBody>({ 0, rp3d::BodyType::STATIC, true, NULL })
            .set<Collider>({ {1.5, 0.2, 0.7}, rp3d::CollisionShapeName::BOX, false, 0.5, NULL })
            .set<Renderer>({ Models::WOODSHELF });
    };
    _Prefabs[PREFABS::CUBE] = [this](const char* name) {
        return ECSWorld.entity(name)
            .set<Prefab>({ PREFABS::CUBE })
            .add<Transform>()
            .set<RigidBody>({ 10.0f, rp3d::BodyType::DYNAMIC, true, NULL })
            .set<Collider>({ {1.5, 1.5, 1.5}, rp3d::CollisionShapeName::BOX, false, 0.5, NULL })
            .add<Velocity>()
            .set<Renderer>({ Models::SUZANNE });
    };

    _Prefabs[PREFABS::BRICK] = [this](const char* name) {
        return ECSWorld.entity(name)
            .set<Prefab>({ PREFABS::BRICK })
            .add<Transform>()
            .set<RigidBody>({ 0, rp3d::BodyType::STATIC, true, NULL })
            .set<Collider>({ {8.5, 0.5,8.5}, rp3d::CollisionShapeName::BOX, false, 0.1, NULL })
            .set<Renderer>({ Models::BRICK });
    };
}

void GameEngine::_TestEcs()
{
    InstantiateEntity(PREFABS::MONKEY)
        .set<Transform>({ {8, 8, 0} })
        .set<Velocity>({ {-1, 0, 0}, 1 });
    InstantiateEntity(PREFABS::MONKEY)
        .set<Transform>({ {-8, 8, 0} })
        .set<Velocity>({ {1, 0, 0}, 1 });
    InstantiateEntity(PREFABS::WOODPLATFORM)
        .set<Transform>({ { 4, 4, 0 } });
    InstantiateEntity(PREFABS::GRASSBLOCK)
        .set<Transform>({ { -8, 0, 0 } });
    InstantiateEntity(PREFABS::GRASSBLOCK)
        .set<Transform>({ { -6, 0, 0 } });
    InstantiateEntity(PREFABS::GRASSBLOCK)
        .set<Transform>({ { -4, 0, 0 } });
    InstantiateEntity(PREFABS::GRASSBLOCK)
        .set<Transform>({ { -2, 0, 0 } });
    InstantiateEntity(PREFABS::GRASSBLOCK)
        .set<Transform>({ { 0, 0, 0 } });
    InstantiateEntity(PREFABS::GRASSBLOCK)
        .set<Transform>({ { 2, 0, 0 } });
    InstantiateEntity(PREFABS::GRASSBLOCK)
        .set<Transform>({ { 4, 0, 0 } });
    InstantiateEntity(PREFABS::GRASSBLOCK)
        .set<Transform>({ { 6, 0, 0 } });
    InstantiateEntity(PREFABS::GRASSBLOCK)
        .set<Transform>({ { 8, 0, 0 } });
    
    ECSWorld.entity("coin")
        .set<Transform>({ {4, 2, 0} })
        .set<RigidBody>({ 0, rp3d::BodyType::STATIC, NULL })
        .set<Collider>({ {0.4, 1, 1}, rp3d::CollisionShapeName::BOX, false, NULL })
        .set<AngularVelocity>({rp3d::Quaternion::fromEulerAngles(0, 0.03f, 0), 1.0f})
        .set<Renderer>({Models::COIN});
    
    ECSWorld.entity("testball")
        .set<Transform>({ {0, 4, 6} })
        .set<AngularVelocity>({rp3d::Quaternion::fromEulerAngles(-0.005f, 0, 0), 1.0f})
        .set<Renderer>({Models::SPHERE});
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
    if(IsPhysicsActive) Accumulator += DeltaTime;

    ECSWorld.progress();
}

flecs::entity GameEngine::InstantiateEntity(PREFABS prefab, const char* name)
{
    flecs::entity entity = _Prefabs[prefab](name);
    Entities.push_back(entity);
    return entity;
}

void GameEngine::DeleteEntity(flecs::entity entity)
{
	Entities.erase(std::remove(Entities.begin(), Entities.end(), entity), Entities.end());
    ecs_defer_begin(ECSWorld);
	entity.destruct();
    if (entity.has<RigidBody>())
    {
        auto rigidbody = entity.get_mut<RigidBody>();
        PhysicsWorld->destroyRigidBody(rigidbody->Body);
        rigidbody->Body = nullptr;
    }
    if (entity.has<CollisionBody>())
    {
        auto collisionBody = entity.get_mut<CollisionBody>();
        PhysicsWorld->destroyCollisionBody(collisionBody->Body);
        collisionBody->Body = nullptr;
    }
    ecs_defer_end(ECSWorld);
}

flecs::entity GameEngine::EntityFromId(flecs::entity_t id)
{
    return ECSWorld.entity(id);
}

flecs::entity GameEngine::SelectedEntity()
{
    return EntityFromId(SelectedEntityId);
}

rp3d::Vector3 GameEngine::WorldToCameraSpace(rp3d::Vector3 position)
{
    GameEngine &engine = GameEngine::GetInstance();
    glm::vec4 positionFromCamera = engine._Camera->ViewMatrix() * glm::vec4(position.x, position.y, position.z, 1);
    positionFromCamera = positionFromCamera / positionFromCamera.w;
    return rp3d::Vector3(positionFromCamera.x, positionFromCamera.y, positionFromCamera.z);
}

rp3d::Vector3 GameEngine::CameraToWorldSpace(rp3d::Vector3 position)
{
    GameEngine& engine = GameEngine::GetInstance();
    glm::vec4 positionInWorld = glm::inverse(engine._Camera->ViewMatrix()) * glm::vec4(position.x, position.y, position.z, 1);
    positionInWorld = positionInWorld / positionInWorld.w;
    return rp3d::Vector3(positionInWorld.x, positionInWorld.y, positionInWorld.z);
}

glm::vec3 GameEngine::WorldToScreenSpace(rp3d::Vector3 position)
{
    GameEngine &engine = GameEngine::GetInstance();
    auto x = glm::vec4(position.x, position.y, position.z, 1);
	glm::vec4 positionFromScreen = engine._Camera->Matrix() * x;
	positionFromScreen = positionFromScreen / positionFromScreen.w;
    return glm::vec3(positionFromScreen.x, positionFromScreen.y, positionFromScreen.z);
}

rp3d::Vector3 GameEngine::ScreenToWorldSpace(glm::vec3 screenPoint)
{
    GameEngine& engine = GameEngine::GetInstance();
    auto I = glm::inverse(engine._Camera->Matrix());
    auto d = I * glm::vec4(screenPoint.x, screenPoint.y, screenPoint.z, 1.0);
    d /= d.w;
    return rp3d::Vector3(d.x, d.y, d.z);
}

flecs::entity GameEngine::EntityFromBody(rp3d::CollisionBody* body)
{
    flecs::entity entity;
    RaycastTargets.iter([&](flecs::iter& it) {
        auto rb = it.field<RigidBody>(2);
        auto cb = it.field<CollisionBody>(3);
        for (int i = 0; i < it.count(); i++) {
            if (it.is_set(2) && rb[i].Body == body) entity = it.entity(i);
            if (it.is_set(3) && cb[i].Body == body) entity = it.entity(i);
        }
    });
    return entity;
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

    raycastInfo->Entity = engine.EntityFromBody(info.body);
    
    Infos.push_back(raycastInfo);
    return rp3d::decimal(1.0);
}

void PhysicsEventListener::onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData)
{
    GameEngine& engine = GameEngine::GetInstance();

    auto count = callbackData.getNbOverlappingPairs();
    for (size_t i = 0; i < count; i++)
    {
        auto pair = callbackData.getOverlappingPair(i);
        auto body1 = pair.getBody1();
        auto body2 = pair.getBody2();
        auto entity1 = engine.EntityFromBody(body1);
        auto entity2 = engine.EntityFromBody(body2);
        if (entity1.has<Prefab>() && entity2.has<Prefab>())
        {
            auto prefab1 = entity1.get<Prefab>();
            auto prefab2 = entity2.get<Prefab>();
            if (prefab1->Prefab == PREFABS::COIN && prefab2->Prefab == PREFABS::PLAYER)
            {
                engine.DeleteEntity(entity1);
            }
            if (prefab2->Prefab == PREFABS::COIN && prefab1->Prefab == PREFABS::PLAYER)
            {
                engine.DeleteEntity(entity2);
            }
        }
    }
}

std::vector<RaycastInfo*> GameEngine::RaycastFromCamera(glm::vec2 screenPoint, rp3d::decimal maxDistance)
{
    GameEngine &engine = GameEngine::GetInstance();
    CameraInfos camera = *engine._Camera;
    auto cameraPos = _Camera->CameraEntity.get<Transform>()->Position;
    rp3d::Vector3 origin{ cameraPos.x, cameraPos.y, cameraPos.z };
    auto pos = engine.ScreenToWorldSpace(glm::vec3(screenPoint.x, screenPoint.y, 0.95));
    //DEBUGGO.get_mut<Transform>()->Position = rp3d::Vector3(pos.x, pos.y, pos.z);

    rp3d::Vector3 direction = pos - origin;
    direction.normalize();
    rp3d::Vector3 end = origin + direction * maxDistance;
    rp3d::Ray ray{ origin, end };

    GatherAllRaycastCallback callback;
    PhysicsWorld->raycast(ray, &callback);

	return callback.Infos;
}

void GameEngine::SetIsEditor(bool isEditor)
{
    IsPhysicsActive = !isEditor;
    IsKinematicActive = !isEditor;
    ShowBoundingBoxes = isEditor;
    IsEditor = isEditor;
}

void GameEngine::SerializeEntities(const char* filename) {
    flecs::entity_to_json_desc_t serializer;
    serializer.serialize_path = true;
    serializer.serialize_values = true;
    struct stat sb;
    std::string RootName = "./resources/scene";
    int ret = stat(RootName.c_str(), &sb);
    int tmp = errno;
    if (stat(RootName.c_str(), &sb) != 0)
    {
        std::filesystem::create_directory(RootName);
    };

    std::ofstream Outfile;
    std::string entitiesString;
    for (auto entity : Entities) {
        entitiesString += "ENTITY\n";
        entitiesString += entity.name();
        entitiesString += "\n";
        rp3d::Vector3 position = entity.get<Transform>()->Position;
        entitiesString += position.to_string();
        entitiesString += "\n";
        rp3d::Quaternion rotation = entity.get<Transform>()->Rotation;
        entitiesString += rotation.to_string();
        entitiesString += "\n";
        PREFABS prefabs = entity.get<Prefab>()->Prefab;
        entitiesString += std::to_string(prefabs);
        entitiesString += "\n";
    }
    std::string filePath = RootName + "/" + filename;
    Outfile.open(filePath, std::ios::out);
    Outfile.write(entitiesString.c_str(), entitiesString.size());
    Outfile.close();
}

PREFABS GetPrefabFromInt(int X)
{
    PREFABS p = (PREFABS) X;
    return p;
}

void  GameEngine::DeserializeEntities(const char* filename)
{
    struct stat sb;
    std::string RootName = "./resources/scene";
    int ret = stat(RootName.c_str(), &sb);
    int tmp = errno;
    if (stat(RootName.c_str(), &sb) != 0)
    {
        std::cout << "Scene directory does not exist";
        return;
    };
    std::string filePath = RootName + "/" + filename;
    std::ifstream Infile;
    Infile.open(filePath, std::ios::in);
    std::string line;

    while (std::getline(Infile, line))
    {
        std::getline(Infile, line);
        std::string name = line;
        std::getline(Infile, line);
        rp3d::Vector3 position;
        sscanf_s(line.c_str(), "Vector3(%f,%f,%f)", &position.x, &position.y, &position.z);
        std::getline(Infile, line);
        rp3d::Quaternion rotation;
        sscanf_s(line.c_str(), "Quaternion(%f,%f,%f,%f)", &rotation.x, &rotation.y, &rotation.z, &rotation.w);
        std::getline(Infile, line);
        int tmpprefab;
        sscanf_s(line.c_str(), "%d", &tmpprefab);
        PREFABS prefab = GetPrefabFromInt(tmpprefab);

        GameEngine::GetInstance().InstantiateEntity(prefab, name.c_str())
            .set<Transform>({ position, rotation });

    };
    Infile.close();
}