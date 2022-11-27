#pragma once
#include "defines.h"
#include "shader_module.h"
#include "ioutils.h"

extern VkPipelineVertexInputStateCreateInfo getPipelineVertexInputStateCreateInfo(
	const uint32_t vertexBindingDescriptionCount,
	const VkVertexInputBindingDescription* vertexBindingDescriptions,
	const uint32_t vertexAttributeDescriptionCount,
	const VkVertexInputAttributeDescription* pVertexAttributeDescriptions
);

extern VkPipelineInputAssemblyStateCreateInfo getPipelineInputAssemblyStateCreateInfo(const VkPrimitiveTopology topology, const VkBool32 primitiveRestartEnable);

extern VkPipelineShaderStageCreateInfo getPipelineShaderStageCreateInfo(const VkShaderModule& shaderModule, const VkShaderStageFlagBits stageFlag);

extern VkPipelineDynamicStateCreateInfo getPipelineDynamicStateCreateInfo(const std::vector<VkDynamicState>& dynamicStates);

extern VkPipelineViewportStateCreateInfo getPipelineViewportStateCreateInfo(
	const uint32_t viewportCount,
	const VkViewport* viewports,
	const uint32_t scissorCount,
	const VkRect2D* scissors
);

extern VkPipelineRasterizationStateCreateInfo getPipelineRasterizationStateCreateInfo(
	const VkBool32 depthClampEnable,
	const VkBool32 rasterizerDiscardEnable,
	const VkPolygonMode polygonMode,
	const float lineWidth,
	const VkCullModeFlags cullMode,
	const VkFrontFace frontFace
);

extern VkPipelineMultisampleStateCreateInfo getPipelineMultisampleStateCreateInfo(
	const VkBool32 sampleShadingEnable,
	const VkSampleCountFlagBits rasterizationSamples,
	const float minSampleShading,
	const VkSampleMask* sampleMask,
	const VkBool32 alphaToCoverageEnable,
	const VkBool32 alphaToOneEnable
);

extern VkPipelineColorBlendAttachmentState getPipelineColorBlendAttachmentState(
	const VkColorComponentFlags colorComponents,
	const VkBool32 blendEnable,
	const VkBlendFactor srcColorBlendFactor,
	const VkBlendFactor dstColorBlendFactor,
	const VkBlendOp colorBlendOp,
	const VkBlendFactor srcAlphaBlendFactor,
	const VkBlendFactor dstAlphaBlendFactor,
	const VkBlendOp alphaBlendOp
);

extern VkPipelineColorBlendStateCreateInfo getPipelineColorBlendStateCreateInfo(
	const VkBool32 logicOpEnable,
	const VkLogicOp logicOp,
	const uint32_t attachmentCount,
	const VkPipelineColorBlendAttachmentState* colorBlendAttachment,
	const float blendConstant0,
	const float blendConstant1,
	const float blendConstant2,
	const float blendConstant3
);