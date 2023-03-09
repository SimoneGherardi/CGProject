# include "DynamicBufferAllocator.h"

void DynamicBufferAllocator::_ReallocateMemory(const VkDeviceSize chunks)
{
	const VkDeviceSize totalSize = chunks * _ChunkSize;

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = totalSize;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.memoryTypeIndex = findMemoryType(
		_PhysicalDevice,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		totalSize
	);

	VkDeviceMemory newLocation;
	CheckVkResult(vkAllocateMemory(
		_Device,
		&memoryAllocateInfo,
		nullptr,
		&newLocation
	));

	if (_DeviceMemory != nullptr)
	{
		_TransferMemoryTo(newLocation);
	}

	_ChunksCount = chunks;
	_DeviceMemory = newLocation;
}

DynamicBufferAllocator::DynamicBufferAllocator(
	const VkPhysicalDevice physicalDevice,
	const VkDevice device,
	const VkDeviceSize chunkSize
) : _PhysicalDevice(physicalDevice), _Device(device), _ChunkSize(chunkSize)
{
	_ReallocateMemory(1);
}

void DynamicBufferAllocator::InnerAllocate(MemoryReference* reference, const BufferAllocationInfo_T bufferAllocationInfo)
{
	if (_ChunkSize * _ChunksCount < _MemoryOffset + bufferAllocationInfo.Size()) {
		VkDeviceSize newCount = (_MemoryOffset + bufferAllocationInfo.Size()) / _ChunkSize + 1;
		_ReallocateMemory(newCount);
	}
	_MemoryOffset = allocateBuffer(
		reference,
		_Device,
		bufferAllocationInfo,
		_DeviceMemory,
		_MemoryOffset
	);
}