#pragma once
#include "defines.h"

extern VkDescriptorSetLayoutBinding getDescriptorSetLayoutBinding(
	const uint32_t binding,
	const VkDescriptorType descriptorType,
	const uint32_t descriptorCount,
	const VkShaderStageFlags stageFlags,
	const VkSampler* immutableSamplers
);

extern void initializeDescriptorSetLayout(
	const VkDevice device,
	const uint32_t bindingsCount,
	const VkDescriptorSetLayoutBinding* bindingsData,
	VkDescriptorSetLayout* descriptorSetLayout
);

extern void cleanupDescriptorSetLayout(
	const VkDevice device,
	const VkDescriptorSetLayout layout
);