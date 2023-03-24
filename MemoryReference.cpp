#include "MemoryReference.h"

void AbstractMemoryReference::Allocate(
	const VulkanContext context,
	const VkDeviceMemory deviceMemory,
	const VkDeviceSize size,
	const VkBufferUsageFlags usage,
	const VkDeviceSize memoryOffset,
	void* data
)
{
	if (IsAllocated)
	{
		throw std::runtime_error("Memory reference already allocated");
	}
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
	Context = context;
	Buffer = buf;
	Memory = deviceMemory;
	Offset = memoryOffset;
	Size = size;
	Usage = usage;
	Data = data;
	IsAllocated = true;
}

void AbstractMemoryReference::Cleanup()
{
	vkDestroyBuffer(Context.Device, Buffer, nullptr);
}

void HostLocalMemoryReference::Transfer()
{
	void* dst;
	vkMapMemory(Context.Device, Memory, Offset, Size, 0, &dst);
	memcpy((int8_t*)dst, (int8_t*)Data, Size);
	vkUnmapMemory(Context.Device, Memory);
}