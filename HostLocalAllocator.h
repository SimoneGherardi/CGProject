#pragma once
#include "Allocator.h"
#include "MemoryReference.h"

class HostLocalAllocator : public Allocator<HostLocalMemoryReference>
{
public:
	HostLocalAllocator(
		const VulkanContext context,
		const VkDeviceSize chunkSize,
		const bool resizable = true
	);

protected:
	DeviceMemory Memory;

	DeviceMemory GetDeviceMemory() override;
	void SetDeviceMemory(DeviceMemory m) override;
	DeviceMemory NewDeviceMemory(const VkDeviceSize chunks) override;
	HostLocalMemoryReference NewReference(
		const VkDeviceMemory deviceMemory,
		const VkDeviceSize size,
		const VkBufferUsageFlags usage,
		const VkDeviceSize memoryOffset,
		void* data
	) override;
};