#include "Allocator.h"

#define ALIGNMENT 0x40

template <typename T>
Allocator<T>::Allocator(
	const VulkanContext context,
	const VkDeviceSize chunkSize,
	const bool resizable
) :
	_Context(context),
	_ChunkSize((int)ceil((float)chunkSize / ALIGNMENT) * ALIGNMENT),
	_Resizable(resizable),
	_Immediate(context),
	_ChunksCount(1)
{
	static_assert(std::is_base_of<AbstractMemoryReference, T>::value, "T must be derived from AbstractMemoryReference");
}

template <typename T>
T Allocator<T>::Get(void* data)
{
	return _Allocations[data];
}

template <typename T>
T Allocator<T>::Allocate(
	void* data,
	const VkDeviceSize sizeRaw,
	const VkBufferUsageFlags usage
)
{
	VkDeviceSize size = (int)ceil((float)sizeRaw / ALIGNMENT) * ALIGNMENT;
	if (_MemoryOffset + size > _ChunksCount * _ChunkSize)
	{
		VkDeviceSize s = (_MemoryOffset + size) / _ChunkSize + 1;
		_ReallocateMemory(s);
	}
	auto newRef = NewReference(
		GetDeviceMemory().Memory,
		size,
		usage,
		_MemoryOffset,
		data
	);
	_Allocations[data] = newRef;
	_MemoryOffset += size;
	return Get(data);
}

template <typename T>
EnqueueFunction_T Allocator<T>::_TransferFromMemoryReference(
	const T source,
	const T destination
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

template <typename T>
void Allocator<T>::_ReallocateMemory(const VkDeviceSize chunks)
{
	LOGDBG("Reallocating " << chunks << " chunks");
	if (!_Resizable) {
		throw std::runtime_error("Allocator is unresizable");
	}
	auto oldMemory = GetDeviceMemory();
	auto oldChunks = _ChunksCount;

	auto newMemory = NewDeviceMemory(chunks);
	std::vector<EnqueueFunction_T> funcs = {};
	std::vector<T> oldRefs = {};
	for (auto& r : _Allocations)
	{
		auto oldRef = r.second;
		auto newRef = NewReference(
			newMemory.Memory,
			oldRef.Size,
			oldRef.Usage,
			oldRef.Offset,
			oldRef.Data
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
		r.Cleanup();
	}
	oldMemory.Cleanup();
	SetDeviceMemory(newMemory);
	_ChunksCount = chunks;
}

template <typename T>
void Allocator<T>::Cleanup()
{
	for (auto& r : _Allocations)
	{
		r.second.Cleanup();
	}
	GetDeviceMemory().Cleanup();
	_Immediate.Cleanup();
}