#include "MemoryBlock.h"

VkDeviceSize _getAligned(VkDeviceSize size, VkDeviceSize alignment)
{
	if (size % alignment == 0) return size;
	return (size / alignment + 1) * alignment;
}

VkDeviceSize MemoryBlock::FindAvailableOffset(VkDeviceSize size, VkDeviceSize alignment)
{
	VkDeviceSize offset = 0;
	for (auto& allocation : Allocations) {
		if (allocation.first - offset >= size) {
			return offset;
		}
		offset = _getAligned(allocation.first + allocation.second, alignment);
	}
	if (Size - offset >= size) {
		return offset;
	}
	throw "NO MORE AVAILABLE BLOCKS";
}

VkDeviceSize MemoryBlock::Allocate(VkDeviceSize size, VkDeviceSize alignment)
{
	VkDeviceSize offset = FindAvailableOffset(size, alignment);
	Allocations[offset] = size;
	return offset;
}

void MemoryBlock::Free(VkDeviceSize offset)
{
	Allocations.erase(offset);
}