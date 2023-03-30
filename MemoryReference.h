#pragma once
#include "defines.h"

struct AbstractMemoryReference
{
protected:
	AbstractMemoryReference() = default;
public:

	void* Data = nullptr;
	VulkanContext Context = {};
	VkDeviceMemory Memory = {};
	VkDeviceSize Size = {};
	VkDeviceSize Offset = {};
	VkBuffer Buffer = {};
	VkBufferUsageFlags Usage = {};
	bool IsAllocated = false;

	void Allocate(
		const VulkanContext context,
		const VkDeviceMemory deviceMemory,
		const VkDeviceSize size,
		const VkBufferUsageFlags usage,
		const VkDeviceSize memoryOffset,
		void* data
	);

	void Cleanup();

	virtual void Transfer() = 0;
};

struct HostLocalMemoryReference : public AbstractMemoryReference
{
	void Transfer();
};