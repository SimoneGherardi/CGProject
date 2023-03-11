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
	bool deviceLocal
) : Context(context), Size(size), IsDeviceLocal(deviceLocal), Memory(nullptr)
{
	auto flag = IsDeviceLocal ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = Size;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.memoryTypeIndex = findMemoryType(
		Context.PhysicalDevice,
		flag,
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
