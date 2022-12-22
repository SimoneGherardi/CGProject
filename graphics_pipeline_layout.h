#pragma once
#include "defines.h"

extern void initializeGraphicsPipelineLayout(
	const VkDevice device,
	const uint32_t setLayoutCount,
	const VkDescriptorSetLayout* setLayouts,
	const uint32_t pushConstantRangeCount,
	const VkPushConstantRange* pPushConstantRanges,
	VkPipelineLayout* pipelineLayout
);

extern void cleanupGraphicsPipelineLayout(const VkDevice device, const VkPipelineLayout pipelineLayout);