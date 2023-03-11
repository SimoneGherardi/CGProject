#pragma once
#include "defines.h"

typedef std::function<void(VkCommandBuffer cmd)> EnqueueFunction_T;

class ImmediateCommandBuffer
{
private:
	const VulkanContext _Context;
	VkFence _Fence;
	VkCommandPool _Pool;
	VkCommandBuffer _Buffer;
public:
	ImmediateCommandBuffer(
		const VulkanContext context
	);
	void Submit(EnqueueFunction_T&& function) const;
	void Wait() const;
	void Cleanup() const;
};