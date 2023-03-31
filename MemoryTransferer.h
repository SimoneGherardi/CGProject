#pragma once
#include "defines.h"
#include "DeviceMemory.h"

struct MemoryTransferer
{
	VulkanContext Context;
	const void* Source;
	Buffer Destination;
	VkDeviceSize SourceSize;

	MemoryTransferer(const VulkanContext context, const Buffer destination, const void* source, const VkDeviceSize srcSize);
	void TransferMapped(const VkDeviceSize dstOffset = 0);
	void TransferStaged(const Buffer stagingBuffer, const VkDeviceSize stagingBufferSize, const VkDeviceSize dstOffset = 0);
};