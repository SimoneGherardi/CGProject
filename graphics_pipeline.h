#pragma once
#include "defines.h"

extern void initializeGraphicsPipeline(
	const VkDevice& device,
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
	const VkPipelineLayout& pipelineLayout,
	const VkRenderPass& renderPass,
	VkPipeline* pipeline
);

extern void cleanupGraphicsPipeline(
	const VkDevice& device,
	const VkPipeline& pipeline
);