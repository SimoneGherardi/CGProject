#include "MemoryTransferer.h"
#include "ImmediateCommandBuffer.h"
#include <glm/gtx/string_cast.hpp>

MemoryTransferer::MemoryTransferer(const VulkanContext context, const Buffer destination, const void* source, const VkDeviceSize srcSize) :
	Context(context),
	Source(source),
	Destination(destination),
	SourceSize(srcSize)
{
}

void MemoryTransferer::TransferMapped(const VkDeviceSize dstOffset)
{
	void* data;
	vkMapMemory(Context->Device, Destination.Memory, Destination.Offset + dstOffset, SourceSize, 0, &data);
	memcpy(data, Source, SourceSize);
	vkUnmapMemory(Context->Device, Destination.Memory);
}

void MemoryTransferer::TransferStaged(const Buffer stagingBuffer, const VkDeviceSize dstOffset)
{
	auto stagingBufferSize = stagingBuffer.Size;
	ImmediateCommandBuffer cmd = ImmediateCommandBuffer(Context);
	VkDeviceSize counts = SourceSize / stagingBufferSize;
	VkDeviceSize remainder = SourceSize % stagingBufferSize;
	for (VkDeviceSize i = 0; i < counts; i++) {
		MemoryTransferer helper = MemoryTransferer(Context, stagingBuffer, (uint8_t*) Source + i * stagingBufferSize, stagingBufferSize);
		helper.TransferMapped();
		cmd.Submit([&](VkCommandBuffer cmd) {
			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = dstOffset + i * stagingBufferSize;
			copyRegion.size = stagingBufferSize;
			vkCmdCopyBuffer(cmd, stagingBuffer.Buffer, Destination.Buffer, 1, &copyRegion);
		});
		cmd.Wait();
	}
	if (remainder > 0) {
		MemoryTransferer helper = MemoryTransferer(Context, stagingBuffer, (uint8_t*) Source + counts * stagingBufferSize, remainder);
		helper.TransferMapped();
		cmd.Submit([&](VkCommandBuffer cmd) {
			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = dstOffset + counts * stagingBufferSize;
			copyRegion.size = remainder;
			vkCmdCopyBuffer(cmd, stagingBuffer.Buffer, Destination.Buffer, 1, &copyRegion);
		});
		cmd.Wait();
	}
}