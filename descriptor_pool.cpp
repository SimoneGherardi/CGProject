#include "descriptor_pool.h"

void initializeDescriptorPool(
	const VulkanContext context,
	const std::vector<VkDescriptorPoolSize> sizes,
	VkDescriptorPool* pool
)
{
	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = 0;
	pool_info.maxSets = 10;
	pool_info.poolSizeCount = (uint32_t)sizes.size();
	pool_info.pPoolSizes = sizes.data();
	vkCreateDescriptorPool(context->Device, &pool_info, nullptr, pool);
}

void cleanupDescriptorPool(
	const VulkanContext context,
	const VkDescriptorPool pool
)
{
	vkDestroyDescriptorPool(context->Device, pool, nullptr);
}