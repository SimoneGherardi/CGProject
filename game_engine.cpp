#include "game_engine.h"

GameEngine::GameEngine()
{
    PhysicsWorld = PhysicsCommon.createPhysicsWorld();
}

GameEngine& GameEngine::GetInstance()
{
    static GameEngine instance;
    return instance;
}

void GameEngine::Loop()
{
    _CurrentFrameTime = std::chrono::system_clock::now();
    std::chrono::duration<double> _DeltaTime = _CurrentFrameTime - _PreviousFrameTime;
    _PreviousFrameTime = _CurrentFrameTime;
    Accumulator += _DeltaTime;

    while (Accumulator >= PHYSIC_TIMESTEP)
    {
        PhysicsWorld->update(PHYSIC_TIMESTEP.count());
        Accumulator -= PHYSIC_TIMESTEP;
    }
}