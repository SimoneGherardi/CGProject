#include "Allocator.h"

Allocator::Allocator(
	const VulkanContext context,
	const VkDeviceSize chunkSize,
	const bool resizable
) :
	_Context(context),
	_ChunkSize(chunkSize),
	_Resizable(resizable),
	_Immediate(context),
	_Memory(DeviceMemory(context, chunkSize * 1, false)),
	_ChunksCount(1)
{}

BufferMemoryReference Allocator::Get(void* data)
{
	return _Allocations[data];
}

BufferMemoryReference Allocator::Allocate(
	void* data,
	const VkDeviceSize size,
	const VkBufferUsageFlags usage
)
{
	if (_MemoryOffset + size > _ChunksCount * _ChunkSize)
	{
		auto s = (_MemoryOffset + size) / _ChunkSize + 1;
		_ReallocateMemory(s);
	}
	auto newRef = BufferMemoryReference::Allocate(
		_Context,
		_Memory.Memory,
		size,
		usage,
		_MemoryOffset
	);
	_Allocations[data] = newRef;
	_MemoryOffset += size;
	return Get(data);
}

BufferMemoryReference Allocator::AllocateAndSet(
	void* data,
	const VkDeviceSize size,
	const VkBufferUsageFlags usage
)
{
	Allocate(data, size, usage);
	auto dst = Get(data);
	TransferFromHost(data, 0, size);
	return dst;
}

void Allocator::TransferFromHost(
	void* data,
	const VkDeviceSize offset,
	const VkDeviceSize size
)
{
	auto destination = Get(data);
	void* dst;
	vkMapMemory(_Context.Device, _Memory.Memory, destination.Offset, destination.Size, 0, &dst);
	memcpy((int8_t*) dst + offset, (int8_t*) data + offset, size);
	vkUnmapMemory(_Context.Device, _Memory.Memory);
}

EnqueueFunction_T Allocator::_TransferFromMemoryReference(
	const BufferMemoryReference source,
	const BufferMemoryReference destination
) const
{
	return [=](VkCommandBuffer cmd) {
		VkBufferCopy copy;
		copy.dstOffset = 0;
		copy.srcOffset = 0;
		copy.size = source.Size;
		vkCmdCopyBuffer(cmd, source.Buffer, destination.Buffer, 1, &copy);
	};
}

void Allocator::_ReallocateMemory(const VkDeviceSize chunks)
{
	LOGDBG("Reallocating " << chunks << " chunks");
	if (!_Resizable) {
		throw "Allocator is unresizable";
	}
	auto oldMemory = _Memory;
	auto oldChunks = _ChunksCount;

	auto newMemory = DeviceMemory(_Context, _ChunkSize * chunks, false);
	std::vector<EnqueueFunction_T> funcs = {};
	std::vector<BufferMemoryReference> oldRefs = {};
	for (auto& r : _Allocations)
	{
		auto oldRef = r.second;
		auto newRef = BufferMemoryReference::Allocate(
			_Context,
			newMemory.Memory,
			oldRef.Size,
			oldRef.Usage,
			oldRef.Offset
		);
		oldRefs.push_back(oldRef);
		r.second = newRef;
		funcs.push_back(_TransferFromMemoryReference(oldRef, newRef));
	}
	_Immediate.Submit([=](auto cmd) {
		for (auto f : funcs)
		{
			f(cmd);
		}
	});
	_Immediate.Wait();
	for (auto r : oldRefs)
	{
		BufferMemoryReference::Cleanup(_Context, r);
	}
	oldMemory.Cleanup();
	_Memory = newMemory;
	_ChunksCount = chunks;
}

void Allocator::Cleanup()
{
	for (auto& r : _Allocations)
	{
		BufferMemoryReference::Cleanup(_Context, r.second);
	}
	_Memory.Cleanup();
	_Immediate.Cleanup();
}
