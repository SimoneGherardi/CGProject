#include "ImmediateCommandBuffer.h"
#include "VulkanStructs.h"

ImmediateCommandBuffer::ImmediateCommandBuffer(
	const VulkanContext context
) : _Context(context)
{
	auto fenceCreateInfo = VulkanStructs::FenceCreateInfo();
	CheckVkResult(
		vkCreateFence(_Context.Device, &fenceCreateInfo, nullptr, &_Fence)
	);

	auto poolCreateInfo = VulkanStructs::CommandPoolCreateInfo();
	CheckVkResult(
		vkCreateCommandPool(_Context.Device, &poolCreateInfo, nullptr, &_Pool)
	);

	auto bufCreateInfo = VulkanStructs::CommandBufferAllocateInfo(_Pool, 1);
	CheckVkResult(
		vkAllocateCommandBuffers(_Context.Device, &bufCreateInfo, &_Buffer)
	);
}

void ImmediateCommandBuffer::Cleanup() const
{
	vkResetCommandPool(_Context.Device, _Pool, 0);
	vkDestroyCommandPool(_Context.Device, _Pool, nullptr);
	vkDestroyFence(_Context.Device, _Fence, nullptr);
}


void ImmediateCommandBuffer::Submit(EnqueueFunction_T&& function) const
{
	VkCommandBuffer cmd = _Buffer;
	auto beginInfo = VulkanStructs::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	CheckVkResult(vkBeginCommandBuffer(cmd, &beginInfo));

	function(cmd);
	CheckVkResult(vkEndCommandBuffer(cmd));

	VkSubmitInfo submit = VulkanStructs::SubmitInfo(&cmd);

	CheckVkResult(vkQueueSubmit(_Context.GraphicsQueue, 1, &submit, _Fence));
}

void ImmediateCommandBuffer::Wait() const
{
	vkWaitForFences(_Context.Device, 1, &_Fence, true, (uint64_t) (2 * 1e9));
	vkResetFences(_Context.Device, 1, &_Fence);
}