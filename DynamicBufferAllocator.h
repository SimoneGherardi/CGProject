#pragma once
#pragma once

#include "Allocator.h"

class DynamicBufferAllocator : public Allocator
{
private:
	const VkPhysicalDevice _PhysicalDevice;
	const VkDevice _Device;
	const VkDeviceSize _ChunkSize = 0;
	VkDeviceMemory _DeviceMemory = nullptr;
	VkDeviceSize _MemoryOffset = 0;
	VkDeviceSize _ChunksCount = 0;

	void _TransferMemoryTo(const VkDeviceMemory newLocation) const; // TODO
	void _ReallocateMemory(const VkDeviceSize chunks);
protected:
	void InnerAllocate(MemoryReference* reference, const BufferAllocationInfo_T bufferAllocationInfo);
public:
	DynamicBufferAllocator(
		const VkPhysicalDevice physicalDevice,
		const VkDevice device,
		const VkDeviceSize chunkSize
	);
};