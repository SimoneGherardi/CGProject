#include "Allocator.h"

//uint32_t findMemoryType(
//    VkPhysicalDevice physicalDevice,
//    uint32_t typeFilter,
//    VkMemoryPropertyFlags properties
//)
//{
//    VkPhysicalDeviceMemoryProperties memProperties;
//    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
//
//    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
//        if ((typeFilter & (1 << i)) &&
//            (memProperties.memoryTypes[i].propertyFlags & properties) ==
//            properties) {
//            return i;
//        }
//    }
//
//    throw std::runtime_error("failed to find suitable memory type!");
//}
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
		&DeviceMemory
	));
}