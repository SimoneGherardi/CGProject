#pragma once
#include "defines.h"
#include "MemoryReference.h"

class Allocator
{
private:
	std::map<void*, const MemoryReference*> _Allocations;
public:
	virtual std::vector<MemoryReference> Allocate(std::vector<std::vector<void>> data) = 0;
	void Update(std::vector<void>);
	void Move(void* oldPosition, void* newPosition);
	void AddReference(void* position, void* copiedPosition);
};


class StaticBufferAllocator : public Allocator
{
private:
	VkDeviceMemory DeviceMemory;
public:
	StaticBufferAllocator(
		const VkPhysicalDevice physicalDevice,
		const VkDevice device,
		const VkDeviceSize size
	);
};

extern uint32_t findMemoryType(uint32_t typeFilter,
	VkMemoryPropertyFlags properties);