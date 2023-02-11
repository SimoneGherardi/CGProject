#pragma once
#include "defines.h"

class MemoryReference
{
public:
	VkBuffer Buffer;
	uint32_t Offset;
	VkDeviceMemory Memory;
};