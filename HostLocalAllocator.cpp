#include "HostLocalAllocator.h"

HostLocalAllocator::HostLocalAllocator(
	const VulkanContext context,
	const VkDeviceSize chunkSize,
	const bool resizable
) :
	Allocator(context, chunkSize, resizable),
	Memory(NewDeviceMemory(_ChunksCount))
{
}

DeviceMemory HostLocalAllocator::NewDeviceMemory(const VkDeviceSize chunks)
{
	return DeviceMemory(
		_Context,
		chunks * _ChunkSize,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
}

HostLocalMemoryReference HostLocalAllocator::NewReference(
	const VkDeviceMemory deviceMemory,
	const VkDeviceSize size,
	const VkBufferUsageFlags usage,
	const VkDeviceSize memoryOffset,
	void* data
)
{
	auto ref = HostLocalMemoryReference();
	ref.Allocate(_Context, deviceMemory, size, usage, memoryOffset, data);
	return ref;
}

DeviceMemory HostLocalAllocator::GetDeviceMemory()
{
	return Memory;
}

void HostLocalAllocator::SetDeviceMemory(DeviceMemory m)
{
	Memory = m;
}