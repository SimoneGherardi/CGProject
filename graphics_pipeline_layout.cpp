#include "graphics_pipeline_layout.h"


void initializeGraphicsPipelineLayout(
	const VkDevice device,
	const uint32_t setLayoutCount,
	const VkDescriptorSetLayout* setLayouts,
	const uint32_t pushConstantRangeCount,
	const VkPushConstantRange* pPushConstantRanges,
	VkPipelineLayout* pipelineLayout
)
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = setLayoutCount;
	pipelineLayoutInfo.pSetLayouts = setLayouts;
	pipelineLayoutInfo.pushConstantRangeCount = pushConstantRangeCount;
	pipelineLayoutInfo.pPushConstantRanges = pPushConstantRanges;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void cleanupGraphicsPipelineLayout(const VkDevice device, const VkPipelineLayout pipelineLayout)
{
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}