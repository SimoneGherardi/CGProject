#include "Allocator.h"

uint32_t findMemoryType(
    VkPhysicalDevice physicalDevice,
    uint32_t typeFilter,
    VkMemoryPropertyFlags properties
)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) ==
            properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

StaticBufferAllocator::StaticBufferAllocator(
	const VkPhysicalDevice physicalDevice,
	const VkDevice device,
	const VkDeviceSize size
)
{
    VkMemoryRequirements memoryRequirements = {};
    memoryRequirements.alignment = 16; // ?
    memoryRequirements.memoryTypeBits = 

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = size;
	memoryAllocateInfo.pNext = nullptr;
    memoryAllocateInfo.memoryTypeIndex = findMemoryType(

    )
}