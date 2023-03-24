#pragma once
#include "defines.h"

struct DeviceMemory
{
	VulkanContext Context;
	VkDeviceMemory Memory;
	VkDeviceSize Size;
	bool IsDeviceLocal;

	DeviceMemory(
		VulkanContext context,
		VkDeviceSize size,
		bool deviceLocal
	);

	void Cleanup();
};