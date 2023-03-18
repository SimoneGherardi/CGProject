#pragma once
#include "defines.h"

extern void initializeCommandBuffer(
	const VkDevice device,
	const VkCommandPool commandPool,
	const uint32_t count,
	VkCommandBuffer* commandBuffer
);

extern void beginCommandBuffer(
	const VkCommandBuffer commandBuffer,
	const VkCommandBufferUsageFlags flags,
	const VkCommandBufferInheritanceInfo* inheritanceInfo
);

extern void endCommandBuffer(const VkCommandBuffer commandBuffer);

extern void cleanupCommandBuffer(
	const VkDevice device,
	const VkCommandPool commandPool,
	const uint32_t count,
	VkCommandBuffer* commandBuffer
);