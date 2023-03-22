#include "CommandBuffer.h"
#include "VulkanStructs.h"
#include "command_buffers.h"

CommandBuffer::CommandBuffer(const VulkanContext context, const VkCommandPool pool) :
	_Context(context),
	_Pool(pool)
{
	initializeCommandBuffer(
		_Context.Device, _Pool, 1, &Buffer
	);
}

void CommandBuffer::Begin(const VkCommandBufferUsageFlags flags)
{
	auto info = VulkanStructs::CommandBufferBeginInfo(flags);
	CheckVkResult(
		vkBeginCommandBuffer(Buffer, &info)
	);
}

void CommandBuffer::End()
{
	CheckVkResult(
		vkEndCommandBuffer(Buffer)
	);
}

void CommandBuffer::Submit(
	const VkQueue queue,
	const VkPipelineStageFlags flags,
	const uint32_t waitSemaphoresCount,
	const VkSemaphore* waitSemaphores,
	const uint32_t signalSemaphoresCount,
	const VkSemaphore* signalSemaphores,
	const VkFence submitFence
)
{
	auto submit = VulkanStructs::SubmitInfo(&Buffer);
	submit.pWaitDstStageMask = &flags;
	submit.waitSemaphoreCount = waitSemaphoresCount;
	submit.pWaitSemaphores = waitSemaphores;
	submit.signalSemaphoreCount = signalSemaphoresCount;
	submit.pSignalSemaphores = signalSemaphores;
	CheckVkResult(
		vkQueueSubmit(
			queue, 1, &submit, submitFence
		)
	);
}

void CommandBuffer::Reset()
{
	CheckVkResult(
		vkResetCommandBuffer(Buffer, 0)
	);
}

void CommandBuffer::Cleanup()
{
	cleanupCommandBuffer(_Context.Device, _Pool, 1, &Buffer);
}