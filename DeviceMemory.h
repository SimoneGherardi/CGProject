#pragma once
#include "defines.h"

struct DeviceMemory
{
	VulkanContext Context;
	VkDeviceMemory Memory;
	VkDeviceSize Size;
	VkMemoryPropertyFlags Flags;

	DeviceMemory(
		VulkanContext context,
		VkDeviceSize size,
		VkMemoryPropertyFlags flags
	);

	void Cleanup();
};