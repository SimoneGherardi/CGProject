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

struct Image
{
	VkImage Image;
	VkDeviceSize Size;
	VkDeviceSize AlignedSize;
	VkDeviceSize Offset;
	VkDeviceMemory Memory;
	VkFormat Format;
	VkExtent3D Extent;
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
	Image NewImage(const VkDeviceSize width, const VkDeviceSize height, const VkImageUsageFlags usage, const uint32_t arrayLayers = 1, const VkImageCreateFlags createFlags = 0);
	void FreeBuffer(const Buffer buffer);
	void FreeImage(const Image image);
	void Cleanup();
};