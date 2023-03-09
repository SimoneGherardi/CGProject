#pragma once

#include "Allocator.h"

class StaticBufferAllocator : public Allocator
{
private:
	VkDevice _Device;
	VkDeviceMemory _DeviceMemory = nullptr;
	VkDeviceSize _MemoryOffset = 0;
protected:
	void InnerAllocate(MemoryReference* reference, const BufferAllocationInfo_T bufferAllocationInfo);
public:
	StaticBufferAllocator(
		const VkPhysicalDevice physicalDevice,
		const VkDevice device,
		const VkDeviceSize size
	);
};