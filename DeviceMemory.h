#pragma once
#include "defines.h"
#include "MemoryBlock.h"

#define ALIGNMENT 0x10

struct Buffer
{
	VkBuffer Buffer;
	VkDeviceSize Size;
	VkDeviceSize AlignedSize;
	VkDeviceSize Offset;
	VkDeviceMemory Memory;
};

struct DeviceMemory
{
	VulkanContext Context;
	VkDeviceMemory Memory;
	VkDeviceSize Size;
	VkMemoryPropertyFlags Flags;
	MemoryBlock Blocks;

	DeviceMemory(
		VulkanContext context,
		VkDeviceSize size,
		VkMemoryPropertyFlags flags
	);

	Buffer NewBuffer(const VkDeviceSize size, const VkBufferUsageFlags usage);
	void FreeBuffer(const Buffer buffer);
	void Cleanup();
};