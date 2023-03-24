#pragma once
#include "defines.h"

struct BufferMemoryReference
{
public:
	VkDeviceMemory Memory;
	VkDeviceSize Size;
	VkDeviceSize Offset;
	VkBuffer Buffer;
	VkBufferUsageFlags Usage;

	static BufferMemoryReference Allocate(
		const VulkanContext context,
		const VkDeviceMemory deviceMemory,
		const VkDeviceSize size,
		const VkBufferUsageFlags usage,
		const VkDeviceSize memoryOffset
	);

	static void Cleanup(
		const VulkanContext context,
		const BufferMemoryReference reference
	);
};