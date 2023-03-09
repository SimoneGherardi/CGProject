#include "Allocator.h"

const uint32_t findMemoryType(
	VkPhysicalDevice physicalDevice,
	VkMemoryPropertyFlags properties,
	VkDeviceSize minimumHeapSize
)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		VkMemoryType t = memProperties.memoryTypes[i];
		bool okProperties = (t.propertyFlags & properties) != 0;
		if (okProperties) {
			bool okHeap = memProperties.memoryHeaps[t.heapIndex].size >= minimumHeapSize;
			if (okHeap) {
				return i;
			}
		}
	}
}

const VkDeviceSize allocateBuffer(
	MemoryReference* reference,
	const VkDevice device,
	const BufferAllocationInfo_T bufferAllocationInfo,
	const VkDeviceMemory deviceMemory,
	const VkDeviceSize memoryOffset
)
{
	// populate buffer info
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = bufferAllocationInfo.Size();
	bufferInfo.usage = bufferAllocationInfo.Usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	// create buffer
	CheckVkResult(
		vkCreateBuffer(
			device,
			&bufferInfo,
			nullptr,
			&(reference->Buffer)
		)
	);
	// bind buffer memory
	CheckVkResult(
		vkBindBufferMemory(
			device,
			reference->Buffer,
			deviceMemory,
			memoryOffset
		)
	);
	// set references and update offst
	reference->Offset = memoryOffset;
	reference->Memory = deviceMemory;
	const VkDeviceSize newMemoryOffset = memoryOffset + bufferAllocationInfo.Size();
	return newMemoryOffset;
}

const MemoryReference* Allocator::Allocate(const BufferAllocationInfo_T bufferAllocationInfo)
{
	MemoryReference* reference = new MemoryReference();
	reference->Id = _Counter++;
	InnerAllocate(reference, bufferAllocationInfo);
	_Allocations[reference->Id] = reference;
	return reference;
}

const std::vector<const MemoryReference*> Allocator::Allocate(std::vector<const BufferAllocationInfo_T> data)
{
	std::vector<const MemoryReference*> references;
	for (const BufferAllocationInfo_T& info : data)
	{
		auto tmp = Allocate(info);
		references.push_back(tmp);
	}
	return references;
}