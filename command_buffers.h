#pragma once
#include "defines.h"

extern void initializeCommandBuffer(
	const VkDevice device,
	const VkCommandPool commandPool,
	VkCommandBuffer* commandBuffer
)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

extern void beginCommandBuffer(
	const VkCommandBuffer commandBuffer,
	const VkCommandBufferUsageFlags flags,
	const VkCommandBufferInheritanceInfo* inheritanceInfo
)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = flags; // Optional
	beginInfo.pInheritanceInfo = inheritanceInfo; // Optional

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}
}