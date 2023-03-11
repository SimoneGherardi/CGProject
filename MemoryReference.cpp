#include "MemoryReference.h"

BufferMemoryReference BufferMemoryReference::Allocate(
	const VulkanContext context,
	const VkDeviceMemory deviceMemory,
	const VkDeviceSize size,
	const VkBufferUsageFlags usage,
	const VkDeviceSize memoryOffset
)
{
	BufferMemoryReference reference;
	auto device = context.Device;
	// populate buffer info
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	// create buffer
	VkBuffer buf;
	CheckVkResult(
		vkCreateBuffer(
			device,
			&bufferInfo,
			nullptr,
			&buf
		)
	);
	// bind buffer memory
	CheckVkResult(
		vkBindBufferMemory(
			device,
			buf,
			deviceMemory,
			memoryOffset
		)
	);
	// set references and update offset
	reference.Buffer = buf;
	reference.Memory = deviceMemory;
	reference.Offset = memoryOffset;
	reference.Size = size;
	reference.Usage = usage;

	return reference;
}

void BufferMemoryReference::Cleanup(
	const VulkanContext context,
	const BufferMemoryReference reference
)
{
	vkDestroyBuffer(context.Device, reference.Buffer, nullptr);
}