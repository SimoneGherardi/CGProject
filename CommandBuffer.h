#pragma once
#include "defines.h"

struct CommandBuffer
{
	VkCommandBuffer Buffer;

	CommandBuffer(const VulkanContext context, const VkCommandPool pool);
	void Begin(const VkCommandBufferUsageFlags flags);
	void End();
	void Submit(
		const VkQueue queue,
		const VkPipelineStageFlags flags,
		const uint32_t waitSemaphoresCount,
		const VkSemaphore* waitSemaphores,
		const uint32_t signalSemaphoresCount,
		const VkSemaphore* signalSemaphores,
		const VkFence submitFence = VK_NULL_HANDLE
	);
	void Reset();
	void Cleanup();
private:
	VulkanContext _Context;
	VkCommandPool _Pool;
};