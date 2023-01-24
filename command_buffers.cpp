#pragma once
#include "command_buffers.h"

void initializeCommandBuffer(
	const VkDevice device,
	const VkCommandPool commandPool,
	const uint32_t count,
	VkCommandBuffer* commandBuffer
)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = count;
	auto result = vkAllocateCommandBuffers(device, &allocInfo, commandBuffer);
	if (result != VK_SUCCESS) {
		PrintVkError(result);
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void beginCommandBuffer(
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

void endCommandBuffer(const VkCommandBuffer commandBuffer)
{
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to end recording command buffer!");
	}
}

void cleanupCommandBuffer(
	const VkDevice device,
	const VkCommandPool commandPool,
	const uint32_t count,
	VkCommandBuffer* commandBuffer
)
{
	vkFreeCommandBuffers(device, commandPool, count, commandBuffer);
}