#pragma once
#include "defines.h"
#include "MemoryReference.h"
#include "ImmediateCommandBuffer.h"
#include "DeviceMemory.h"

template <typename T>
class Allocator
{

protected:
	const VulkanContext _Context;
	const VkDeviceSize _ChunkSize;
	const ImmediateCommandBuffer _Immediate;
	const bool _Resizable;

	VkDeviceSize _MemoryOffset = 0;
	VkDeviceSize _ChunksCount = 0;

	std::map<void*, T> _Allocations;

	void _ReallocateMemory(const VkDeviceSize chunks);
	EnqueueFunction_T _TransferFromMemoryReference(
		const T source,
		const T destination
	) const;

	virtual DeviceMemory GetDeviceMemory() = 0;
	virtual void SetDeviceMemory(DeviceMemory m) = 0;
	virtual DeviceMemory NewDeviceMemory(const VkDeviceSize chunks) = 0;
	virtual T NewReference(
		const VkDeviceMemory deviceMemory,
		const VkDeviceSize size,
		const VkBufferUsageFlags usage,
		const VkDeviceSize memoryOffset,
		void* data
	) = 0;

public:
	Allocator(
		const VulkanContext context,
		const VkDeviceSize chunkSize,
		const bool resizable = true
	);

	T Get(void* data);
	T Allocate(
		void* data,
		const VkDeviceSize size,
		const VkBufferUsageFlags usage
	);
	void Cleanup();
};

template class Allocator<HostLocalMemoryReference>;