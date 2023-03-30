#pragma once
#include "defines.h"
#include "Allocator.h"


class LocalAllocator
{
private:
	const VulkanContext _Context;
	const VkDeviceSize _ChunkSize;
	const ImmediateCommandBuffer _Immediate;
	const bool _Resizable;

	DeviceMemory _Memory;
	VkDeviceSize _MemoryOffset = 0;
	VkDeviceSize _ChunksCount = 0;

	std::map<void*, BufferMemoryReference> _Allocations;

	void _ReallocateMemory(const VkDeviceSize chunks);
	EnqueueFunction_T _TransferFromMemoryReference(
		const BufferMemoryReference source,
		const BufferMemoryReference destination
	) const;
public:
	LocalAllocator(
		const VulkanContext context,
		const VkDeviceSize chunkSize,
		const bool resizable = true
	);

	BufferMemoryReference Get(void* data);
	BufferMemoryReference Allocate(
		void* data,
		const VkDeviceSize size,
		const VkBufferUsageFlags usage
	);
	BufferMemoryReference AllocateAndSet(
		void* data,
		const VkDeviceSize size,
		const VkBufferUsageFlags usage
	);
	void TransferFromHost(
		void* data,
		const VkDeviceSize offset,
		const VkDeviceSize size
	);

	void Cleanup();
};