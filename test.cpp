#include "test.h"

void Count(flecs::entity e, Counter& counter)
{
    counter.count++;
}

void Print(flecs::entity e, Counter& counter)
{
    printf("%d\n", counter.count);
}

Test::Test(flecs::world& world)
{
    world.module<Test>("Test");

    m_counter = world.component<Counter>();

    m_count = world.system<Counter>()
        .kind(flecs::PostUpdate)
        .each(Count);
    m_print = world.system<Counter>()
        .kind(flecs::OnUpdate)
        .each(Print);
}