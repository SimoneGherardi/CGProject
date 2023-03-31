#include "DeviceMemory.h"
#include "VulkanStructs.h"

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
) : Context(context), Size(size), Flags(flags), Memory(nullptr), Blocks(size)
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

Buffer DeviceMemory::NewBuffer(const VkDeviceSize size, const VkBufferUsageFlags usage)
{
	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	VkBuffer b;
	auto offset = Blocks.FindAvailableOffset(size);
	if (offset == -1) {
		throw "Cannot allocate buffer";
	}
	CheckVkResult(vkCreateBuffer(
		Context.Device,
		&bufferCreateInfo,
		nullptr,
		&b
	));
	CheckVkResult(vkBindBufferMemory(
		Context.Device,
		b,
		Memory,
		offset
	));
	Blocks.Allocate(size);
	return Buffer{ b, size, offset, Memory };
}

void DeviceMemory::FreeBuffer(const Buffer buffer)
{
	vkDestroyBuffer(Context.Device, buffer.Buffer, nullptr);
	Blocks.Free(buffer.Offset);
}

void DeviceMemory::Cleanup()
{
	vkFreeMemory(Context.Device, Memory, nullptr);
}
