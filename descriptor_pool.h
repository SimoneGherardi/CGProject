#pragma once
#include "defines.h"

extern void initializeDescriptorPool(
	const VulkanContext context,
	const std::vector<VkDescriptorPoolSize> sizes,
	VkDescriptorPool* pool
);

extern void cleanupDescriptorPool(
	const VulkanContext context,
	const VkDescriptorPool pool
);