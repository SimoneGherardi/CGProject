#include "DeviceMemory.h"

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

	throw "Cannot find memory";
}

DeviceMemory::DeviceMemory(
	VulkanContext context,
	VkDeviceSize size,
	VkMemoryPropertyFlags flags
) : Context(context), Size(size), Flags(flags), Memory(nullptr)
{

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = Size;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.memoryTypeIndex = findMemoryType(
		Context.PhysicalDevice,
		flags,
		Size
	);

	VkDeviceMemory m;
	CheckVkResult(vkAllocateMemory(
		Context.Device,
		&memoryAllocateInfo,
		nullptr,
		&m
	));

	Memory = m;
}

void DeviceMemory::Cleanup()
{
	vkFreeMemory(Context.Device, Memory, nullptr);
}
