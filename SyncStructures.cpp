#include "SyncStructures.h"
#include "VulkanStructs.h"

Fence::Fence(const VulkanContext context, const bool signaled)
: _Context(context)
{
	auto info = VulkanStructs::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
	CheckVkResult(
		vkCreateFence(_Context.Device, &info, nullptr, &Instance)
	);
}

void Fence::Wait()
{
	CheckVkResult(
		vkWaitForFences(_Context.Device, 1, &Instance, true, 1000000000)
	);
}

void Fence::Reset()
{
	CheckVkResult(
		vkResetFences(_Context.Device, 1, &Instance)
	);
}
void Fence::Cleanup()
{
	vkDestroyFence(_Context.Device, Instance, nullptr);
}


Semaphore::Semaphore(const VulkanContext context)
: _Context(context)
{
	auto info = VulkanStructs::SemaphoreCreateInfo();
	CheckVkResult(
		vkCreateSemaphore(_Context.Device, &info, nullptr, &Instance)
	);
}

void Semaphore::Cleanup()
{
	vkDestroySemaphore(_Context.Device, Instance, nullptr);
}