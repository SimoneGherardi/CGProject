#pragma once

#include "reactphysics3d/reactphysics3d.h"
#include "ecs_modules.h"

constexpr std::chrono::duration<double> PHYSICS_TIMESTEP = std::chrono::duration<double>(1.0f / 60.0f);

class GameEngine
{
private:
	GameEngine();
	std::chrono::system_clock::time_point _CurrentFrameTime;
	std::chrono::system_clock::time_point _PreviousFrameTime;

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

	void Loop(float delta);

	GameEngine(GameEngine const&) = delete;
	void operator=(GameEngine const&) = delete;
};