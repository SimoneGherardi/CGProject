#include "graphics_pipeline.h"

void initializeGraphicsPipeline(
	const VkDevice device,
	const uint32_t shaderStagesCount,
	const VkPipelineShaderStageCreateInfo* shaderStages,
	const VkPipelineVertexInputStateCreateInfo* vertexInputInfo,
	const VkPipelineInputAssemblyStateCreateInfo* inputAssembly,
	const VkPipelineViewportStateCreateInfo* viewportState,
	const VkPipelineRasterizationStateCreateInfo* rasterizer,
	const VkPipelineMultisampleStateCreateInfo* multisampling,
	const VkPipelineDepthStencilStateCreateInfo* depthStencil,
	const VkPipelineColorBlendStateCreateInfo* colorBlending,
	const VkPipelineDynamicStateCreateInfo* dynamicState,
	const VkPipelineLayout pipelineLayout,
	const VkRenderPass renderPass,
	VkPipeline* pipeline
)
{
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = shaderStagesCount;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = vertexInputInfo;
	pipelineInfo.pInputAssemblyState = inputAssembly;
	pipelineInfo.pViewportState = viewportState;
	pipelineInfo.pRasterizationState = rasterizer;
	pipelineInfo.pMultisampleState = multisampling;
	pipelineInfo.pDepthStencilState = depthStencil;
	pipelineInfo.pColorBlendState = colorBlending;
	pipelineInfo.pDynamicState = dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, pipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}
}

void cleanupGraphicsPipeline(
	const VkDevice device,
	const VkPipeline pipeline
)
{
	vkDestroyPipeline(device, pipeline, nullptr);
}