#pragma once
#include "defines.h"
#include "MemoryReference.h"

struct BufferAllocationInfo_T {
	VkDeviceSize ItemSize;
	VkDeviceSize Count;
	VkBufferUsageFlags Usage;
	VkDeviceSize Size() const { return ItemSize * Count; }
};

class Allocator
{
private:
	std::map<void*, const MemoryReference*> _Allocations;
public:
	virtual std::vector<MemoryReference> Allocate(std::vector<std::vector<BufferAllocationInfo_T>> data) = 0;
	// void Update(std::vector<void>);
	// void Move(void* oldPosition, void* newPosition);
	// void AddReference(void* position, void* copiedPosition);
};


class StaticBufferAllocator : public Allocator
{
private:
	VkDevice _Device;
	VkDeviceMemory _DeviceMemory = nullptr;
	VkDeviceSize _MemoryOffset = 0;

	MemoryReference _Allocate(const BufferAllocationInfo_T bufferAllocationInfo);
public:
	StaticBufferAllocator(
		const VkPhysicalDevice physicalDevice,
		const VkDevice device,
		const VkDeviceSize size
	);

	std::vector<MemoryReference> Allocate(std::vector<BufferAllocationInfo_T> data);
};

extern uint32_t findMemoryType(
	VkPhysicalDevice physicalDevice,
	VkMemoryPropertyFlags properties,
	VkDeviceSize minimumHeapSize
);