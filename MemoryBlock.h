#pragma once
#include "defines.h"

struct MemoryBlock
{
	VkDeviceSize Size;
	std::map<VkDeviceSize, VkDeviceSize> Allocations = {};
	MemoryBlock(VkDeviceSize size) : Size(size) {}
	VkDeviceSize FindAvailableOffset(VkDeviceSize size);
	VkDeviceSize Allocate(VkDeviceSize size);
	void Free(VkDeviceSize offset);
};