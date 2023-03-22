#pragma once
#include "defines.h"

struct Fence
{
	VkFence Instance;

	Fence(const VulkanContext context, const bool signaled);
	void Wait();
	void Reset();
	void Cleanup();

private:
	VulkanContext _Context;
};

struct Semaphore
{
	VkSemaphore Instance;

	Semaphore(const VulkanContext context);
	void Cleanup();

private:
	VulkanContext _Context;
};