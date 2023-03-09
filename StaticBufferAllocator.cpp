# include "StaticBufferAllocator.h"



StaticBufferAllocator::StaticBufferAllocator(
	const VkPhysicalDevice physicalDevice,
	const VkDevice device,
	const VkDeviceSize size
)
{
	_Device = device;

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = size;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.memoryTypeIndex = findMemoryType(
		physicalDevice,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		size
	);

	CheckVkResult(vkAllocateMemory(
		device,
		&memoryAllocateInfo,
		nullptr,
		&_DeviceMemory
	));
}

void StaticBufferAllocator::InnerAllocate(MemoryReference* reference, const BufferAllocationInfo_T bufferAllocationInfo)
{
	_MemoryOffset = allocateBuffer(
		reference,
		_Device,
		bufferAllocationInfo,
		_DeviceMemory,
		_MemoryOffset
	);
}