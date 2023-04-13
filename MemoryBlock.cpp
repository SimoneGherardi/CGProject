#include "MemoryBlock.h"

VkDeviceSize MemoryBlock::FindAvailableOffset(VkDeviceSize size)
{
	VkDeviceSize offset = 0;
	for (auto& allocation : Allocations) {
		if (allocation.first - offset >= size) {
			return offset;
		}
		offset = allocation.first + allocation.second;
	}
	if (Size - offset >= size) {
		return offset;
	}
	throw "NO MORE AVAILABLE BLOCKS";
}

VkDeviceSize MemoryBlock::Allocate(VkDeviceSize size)
{
	VkDeviceSize offset = FindAvailableOffset(size);
	Allocations[offset] = size;
	return offset;
}

void MemoryBlock::Free(VkDeviceSize offset)
{
	Allocations.erase(offset);
}