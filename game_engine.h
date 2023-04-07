#pragma once

#include "reactphysics3d/reactphysics3d.h"
#include "ecs_modules.h"
#include "camera.h"

class GatherAllRaycastCallback : public rp3d::RaycastCallback {
public:
	std::vector <rp3d::RaycastInfo*> Infos;
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

	void _TestEcs();
	void _SetupPhysicsLogger();

public:
	static GameEngine& GetInstance();
	std::chrono::duration<double> DeltaTime = std::chrono::duration<double>::zero();
	std::chrono::duration<double> Accumulator = std::chrono::duration<double>::zero();
	flecs::world ECSWorld;
	rp3d::PhysicsCommon PhysicsCommon;
	rp3d::PhysicsWorld* PhysicsWorld;
	std::chrono::system_clock::time_point GetCurrentFrameTime();

	CameraInfos& Camera();

	void Loop(float delta);

	GameEngine(GameEngine const&) = delete;
	void operator=(GameEngine const&) = delete;

	rp3d::Vector3 WorldToCameraSpace(rp3d::Vector3 position);
	rp3d::Vector3 CameraToWorldSpace(rp3d::Vector3 position);

	glm::vec3 WorldToScreenSpace(rp3d::Vector3 position);
	glm::vec3 ScreenToWorldSpace(glm::vec2 screenPoint);

	std::vector<rp3d::RaycastInfo*> RaycastFromCamera(glm::vec2 screenPoint, rp3d::decimal maxDistance);
};