#pragma once
#include "defines.h"

typedef size_t MemoryReferenceId;

class MemoryReference
{
private:
	bool Freed = false;
public:
	MemoryReferenceId Id;
	VkBuffer Buffer;
	uint32_t Offset;
	VkDeviceMemory Memory;
};