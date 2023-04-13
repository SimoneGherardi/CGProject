#pragma once
#include "defines.h"

extern void initializeDescriptorSet(
	const VulkanContext context,
	const VkDescriptorPool pool,
	const uint32_t layoutsCount,
	const VkDescriptorSetLayout* layouts,
	VkDescriptorSet* descriptorSet
);

extern void updateDescriptorSet(
	const VulkanContext context,
	const VkBuffer buffer,
	const VkDeviceSize size,
	const uint32_t dstBinding,
	const VkDescriptorSet dstSet,
	const uint32_t descriptorCount,
	const VkDescriptorType descriptorType
);

extern void updateDescriptorSetImages(
	const VulkanContext context,
	const uint32_t dstBinding,
	const VkDescriptorSet dstSet,
	const VkDescriptorType type,
	const uint32_t descriptorCount,
	VkDescriptorImageInfo* imageInfo
);