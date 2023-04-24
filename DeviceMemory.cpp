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

bool operator == (const Buffer& lhs, const Buffer& rhs)
{
	return lhs.Buffer == rhs.Buffer;
}

bool operator == (const Image& lhs, const Image& rhs)
{
	return lhs.Image == rhs.Image;
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
	auto result = Buffer{ b, size, alignedSize, offset, Memory };
	Buffers.push_back(result);
	return result;
}

Image DeviceMemory::NewImage(const VkDeviceSize width, const VkDeviceSize height, const VkImageUsageFlags usage, const uint32_t arrayLayers, const VkImageCreateFlags createFlags, const uint32_t mipLevels)
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
		createFlags,
		mipLevels
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
	auto result = Image{ image, size, alignedSize, offset, Memory, format, extent };
	Images.push_back(result);
	return result;
}

void DeviceMemory::FreeBuffer(const Buffer buffer)
{
	vkDestroyBuffer(Context->Device, buffer.Buffer, nullptr);
	Blocks.Free(buffer.Offset);
	Buffers.erase(std::remove(Buffers.begin(), Buffers.end(), buffer), Buffers.end());
}

void DeviceMemory::FreeImage(const Image image)
{
	vkDestroyImage(Context->Device, image.Image, nullptr);
	Blocks.Free(image.Offset);
	Images.erase(std::remove(Images.begin(), Images.end(), image), Images.end());
}

void DeviceMemory::Cleanup()
{
	while (Buffers.size() > 0) {
		auto buffer = Buffers.back();
		FreeBuffer(buffer);
	}
	while (Images.size() > 0) {
		auto image = Images.back();
		FreeImage(image);
	}
	vkFreeMemory(Context->Device, Memory, nullptr);
}
