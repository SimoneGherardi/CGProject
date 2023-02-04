#pragma once

#include "reactphysics3d/reactphysics3d.h"

constexpr std::chrono::duration<double> PHYSIC_TIMESTEP = std::chrono::duration<double>(1.0f / 60.0f);

class GameEngine {
public:
    static GameEngine& GetInstance();
    std::chrono::duration<double> Accumulator = std::chrono::duration<double>::zero();
    rp3d::PhysicsCommon PhysicsCommon;
    rp3d::PhysicsWorld* PhysicsWorld;

    void Loop();

    GameEngine(GameEngine const&) = delete;
    void operator=(GameEngine const&) = delete;
private:
    GameEngine();
    std::chrono::system_clock::time_point _CurrentFrameTime;
    std::chrono::system_clock::time_point _PreviousFrameTime;
    std::chrono::duration<double> _DeltaTime = std::chrono::duration<double>::zero();
};