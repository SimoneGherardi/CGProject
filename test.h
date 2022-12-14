#pragma once
#include <flecs.h>

typedef struct Counter
{
	int count;
} Counter;

class Test
{
private:
    flecs::entity m_counter;
    flecs::entity m_print;
    flecs::entity m_count;
public:
    Test(flecs::world& world);
};