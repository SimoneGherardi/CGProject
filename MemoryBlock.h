#pragma once
#include "defines.h"

struct MemoryBlock
{
	VkDeviceSize Size;
	std::map<VkDeviceSize, VkDeviceSize> Allocations = {};
	MemoryBlock(VkDeviceSize size) : Size(size) {}
	VkDeviceSize FindAvailableOffset(VkDeviceSize size, VkDeviceSize alignment = 0x40);
	VkDeviceSize Allocate(VkDeviceSize size, VkDeviceSize alignment = 0x40);
	void Free(VkDeviceSize offset);
};