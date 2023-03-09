#pragma once
#include "defines.h"
#include "MemoryReference.h"

struct BufferAllocationInfo_T {
	VkDeviceSize ItemSize;
	VkDeviceSize Count;
	VkBufferUsageFlags Usage;
	VkDeviceSize Size() const { return ItemSize * Count; }
};

extern const uint32_t findMemoryType(
	VkPhysicalDevice physicalDevice,
	VkMemoryPropertyFlags properties,
	VkDeviceSize minimumHeapSize
);

extern const VkDeviceSize allocateBuffer(
	MemoryReference* reference,
	const VkDevice device,
	const BufferAllocationInfo_T bufferAllocationInfo,
	const VkDeviceMemory deviceMemory,
	const VkDeviceSize memoryOffset
);

class Allocator
{
private:
	const std::map<const MemoryReferenceId, const MemoryReference*> _Allocations;
	MemoryReferenceId _Counter = 0;
protected:
	virtual void InnerAllocate(MemoryReference* reference, const BufferAllocationInfo_T bufferAllocationInfo) = 0;
public:
	const MemoryReference* Allocate(const BufferAllocationInfo_T bufferAllocationInfo);
	const std::vector<const MemoryReference*> Allocate(const std::vector<const BufferAllocationInfo_T> data);
};