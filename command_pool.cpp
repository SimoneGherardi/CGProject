#include "command_pool.h"

void initializeCommandPool(
	const VkDevice device,
	const QueueFamilyIndices queueFamilyIndices,
	VkCommandPool* commandPool
)
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	auto result = vkCreateCommandPool(device, &poolInfo, nullptr, commandPool);
	if (result != VK_SUCCESS) {
		PrintVkError(result);
		throw std::runtime_error("failed to create command pool!");
	}
}

void cleanupCommandPool(
	const VkDevice device,
	const VkCommandPool commandPool
)
{
	vkDestroyCommandPool(device, commandPool, nullptr);
}