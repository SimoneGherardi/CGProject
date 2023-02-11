#include "Allocator.h"

uint32_t findMemoryType(
	VkPhysicalDevice physicalDevice,
	VkMemoryPropertyFlags properties,
	VkDeviceSize minimumHeapSize
)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		VkMemoryType t = memProperties.memoryTypes[i];
		bool okProperties = (t.propertyFlags & properties) == properties;
		if (okProperties) {
			bool okHeap = memProperties.memoryHeaps[t.heapIndex].size >= minimumHeapSize;
			if (okHeap) {
				return i;
			}
		}
	}
}

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

MemoryReference StaticBufferAllocator::_Allocate(const BufferAllocationInfo_T bufferAllocationInfo)
{
	MemoryReference reference = {};
	// populate buffer info
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = bufferAllocationInfo.Size();
	bufferInfo.usage = bufferAllocationInfo.Usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	// create buffer
	CheckVkResult(
		vkCreateBuffer(
			_Device,
			&bufferInfo,
			nullptr,
			&(reference.Buffer)
		)
	);
	// bind buffer memory
	CheckVkResult(
		vkBindBufferMemory(
			_Device,
			reference.Buffer,
			_DeviceMemory,
			_MemoryOffset
		)
	);
	// set references and update offst
	reference.Offset = _MemoryOffset;
	reference.Memory = _DeviceMemory;
	_MemoryOffset += bufferAllocationInfo.Size();
	return reference;
}

std::vector<MemoryReference> StaticBufferAllocator::Allocate(std::vector<BufferAllocationInfo_T> data)
{
	std::vector<MemoryReference> references;
	for (BufferAllocationInfo_T info : data)
	{
		auto tmp = _Allocate(info);
		references.push_back(tmp);
	}
	return references;
}