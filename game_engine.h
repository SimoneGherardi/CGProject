#pragma once
#define FLECS_INVALID_ENTITY 0

#include "reactphysics3d/reactphysics3d.h"
#include "ecs_modules.h"
#include "camera.h"
#include <map>
#include <optional>

enum PREFABS {
	MONKEY,
	BUSH,
	COIN,
	GRASSBLOCK,
	ROCK1,
	ROCK2,
	SIGN,
	TREE1,
	TREE2,
	WOODBRIDGE,
	WOODPLATFORM,
	WOODSHELF,
	CUBE,
};

struct RaycastInfo : rp3d::RaycastInfo {
	flecs::entity Entity;
};

class GatherAllRaycastCallback : public rp3d::RaycastCallback {
public:
	std::vector <RaycastInfo*> Infos;
	virtual rp3d::decimal notifyRaycastHit(const rp3d::RaycastInfo& info);
};

constexpr std::chrono::duration<double> PHYSICS_TIMESTEP = std::chrono::duration<double>(1.0f / 60.0f);

class GameEngine
{
private:
	GameEngine();
	std::chrono::system_clock::time_point _CurrentFrameTime;
	std::chrono::system_clock::time_point _PreviousFrameTime;

	CameraInfos _Camera;

	std::map<PREFABS, std::function<flecs::entity(const char*)>> _Prefabs;
	void _InitPrefabs();
	void _TestEcs();
	void _SetupPhysicsLogger();

public:
	static GameEngine& GetInstance();
	std::chrono::duration<double> DeltaTime = std::chrono::duration<double>::zero();
	std::chrono::duration<double> Accumulator = std::chrono::duration<double>::zero();
	flecs::world ECSWorld;
	std::vector<flecs::entity> Entities;
	flecs::entity_t SelectedEntityId = FLECS_INVALID_ENTITY;
	flecs::filter<> RaycastTargets;
	rp3d::PhysicsCommon PhysicsCommon;
	rp3d::PhysicsWorld* PhysicsWorld;
	std::chrono::system_clock::time_point GetCurrentFrameTime();

	bool IsEditor = true;
	bool IsPhysicsActive;
	bool IsKinematicActive;
	bool ShowBoundingBoxes;

	CameraInfos& Camera();

	void Loop(float delta);

	GameEngine(GameEngine const&) = delete;
	void operator=(GameEngine const&) = delete;

	flecs::entity InstantiateEntity(PREFABS prefab, const char* name = nullptr);
	flecs::entity EntityFromId(flecs::entity_t id);
	flecs::entity SelectedEntity();

	rp3d::Vector3 WorldToCameraSpace(rp3d::Vector3 position);
	rp3d::Vector3 CameraToWorldSpace(rp3d::Vector3 position);

	glm::vec3 WorldToScreenSpace(rp3d::Vector3 position);
	rp3d::Vector3 ScreenToWorldSpace(glm::vec3 screenPoint);

	std::vector<RaycastInfo*> RaycastFromCamera(glm::vec2 screenPoint, rp3d::decimal maxDistance);

	void SetIsEditor(bool isEditor);
};