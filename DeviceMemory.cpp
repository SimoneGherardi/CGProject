#include "DeviceMemory.h"
#include "VulkanStructs.h"

VkDeviceSize _getAlignedSize(VkDeviceSize size, VkDeviceSize alignment = ALIGNMENT)
{
	if (size % alignment == 0) return size;
	return (size / alignment + 1) * alignment;
}

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
		Context->PhysicalDevice,
		flags,
		Size
	);
	VkDeviceMemory m;
	CheckVkResult(vkAllocateMemory(
		Context->Device,
		&memoryAllocateInfo,
		nullptr,
		&m
	));
	Memory = m;
}

Buffer DeviceMemory::NewBuffer(const VkDeviceSize size, const VkBufferUsageFlags usage)
{
	auto alignedSize = _getAlignedSize(size);
	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = alignedSize;
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	VkBuffer b;
	auto offset = Blocks.FindAvailableOffset(alignedSize);
	CheckVkResult(vkCreateBuffer(
		Context->Device,
		&bufferCreateInfo,
		nullptr,
		&b
	));
	CheckVkResult(vkBindBufferMemory(
		Context->Device,
		b,
		Memory,
		offset
	));
	Blocks.Allocate(alignedSize);
	return Buffer{ b, size, alignedSize, offset, Memory };
}

Image DeviceMemory::NewImage(const VkDeviceSize width, const VkDeviceSize height, const VkImageUsageFlags usage, const uint32_t arrayLayers, const VkImageCreateFlags createFlags)
{
	VkImage image;
	VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
	VkExtent3D extent = {};
	extent.width = width;
	extent.height = height;
	extent.depth = 1;
	VkImageCreateInfo imageInfo = VulkanStructs::ImageCreateInfo(
		format,
		usage,
		extent,
		arrayLayers,
		createFlags
	);

	CheckVkResult(vkCreateImage(
		Context->Device,
		&imageInfo,
		nullptr,
		&image
	));

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(Context->Device, image, &memReqs);
	VkDeviceSize size = memReqs.size;
	auto alignedSize = _getAlignedSize(size, memReqs.alignment);

	auto offset = Blocks.FindAvailableOffset(alignedSize, memReqs.alignment);
	if (offset == -1) {
		throw "Cannot allocate image";
	}

	CheckVkResult(vkBindImageMemory(
		Context->Device,
		image,
		Memory,
		offset
	));

	Blocks.Allocate(alignedSize, memReqs.alignment);
	return Image{ image, size, alignedSize, offset, Memory, format, extent };
}

void DeviceMemory::FreeBuffer(const Buffer buffer)
{
	vkDestroyBuffer(Context->Device, buffer.Buffer, nullptr);
	Blocks.Free(buffer.Offset);
}

void DeviceMemory::FreeImage(const Image image)
{
	vkDestroyImage(Context->Device, image.Image, nullptr);
	Blocks.Free(image.Offset);
}

void DeviceMemory::Cleanup()
{
	vkFreeMemory(Context->Device, Memory, nullptr);
}
