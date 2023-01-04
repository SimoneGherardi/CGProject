#include "graphics_pipeline_states.h"

VkPipelineVertexInputStateCreateInfo getPipelineVertexInputStateCreateInfo(
	const uint32_t vertexBindingDescriptionCount,
	const VkVertexInputBindingDescription* vertexBindingDescriptions,
	const uint32_t vertexAttributeDescriptionCount,
	const VkVertexInputAttributeDescription* pVertexAttributeDescriptions
)
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = vertexBindingDescriptionCount;
	vertexInputInfo.pVertexBindingDescriptions = vertexBindingDescriptions;
	vertexInputInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptionCount;
	vertexInputInfo.pVertexAttributeDescriptions = pVertexAttributeDescriptions;
	return vertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo getPipelineInputAssemblyStateCreateInfo(const VkPrimitiveTopology topology, const VkBool32 primitiveRestartEnable)
{
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = topology;
	inputAssembly.primitiveRestartEnable = primitiveRestartEnable;
	return inputAssembly;
}

VkPipelineShaderStageCreateInfo getPipelineShaderStageCreateInfo(const VkShaderModule shaderModule, const VkShaderStageFlagBits stageFlag)
{
	VkPipelineShaderStageCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.stage = stageFlag;
	createInfo.module = shaderModule;
	createInfo.pName = "main";
	return createInfo;
}

VkPipelineDynamicStateCreateInfo getPipelineDynamicStateCreateInfo(const std::vector<VkDynamicState> dynamicStates)
{
	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();
	return dynamicState;
}

VkPipelineViewportStateCreateInfo getPipelineViewportStateCreateInfo(
	const uint32_t viewportCount,
	const VkViewport* viewports,
	const uint32_t scissorCount,
	const VkRect2D* scissors
)
{
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = viewportCount;
	viewportState.pViewports = viewports;
	viewportState.scissorCount = scissorCount;
	viewportState.pScissors = scissors;
	return viewportState;
}

VkPipelineRasterizationStateCreateInfo getPipelineRasterizationStateCreateInfo(
	const VkBool32 depthClampEnable,
	const VkBool32 rasterizerDiscardEnable,
	const VkPolygonMode polygonMode,
	const float lineWidth,
	const VkCullModeFlags cullMode,
	const VkFrontFace frontFace
)
{
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = depthClampEnable;
	rasterizer.rasterizerDiscardEnable = rasterizerDiscardEnable;
	rasterizer.polygonMode = polygonMode;
	rasterizer.lineWidth = lineWidth;
	rasterizer.cullMode = cullMode;
	rasterizer.frontFace = frontFace;
	/*
		The rasterizer can alter the depth values by adding a constant
		value or biasing them based on a fragment's slope.
		This is sometimes used for shadow mapping.
	*/
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;
	return rasterizer;
}

VkPipelineMultisampleStateCreateInfo getPipelineMultisampleStateCreateInfo(
	const VkBool32 sampleShadingEnable,
	const VkSampleCountFlagBits rasterizationSamples,
	const float minSampleShading,
	const VkSampleMask* sampleMask,
	const VkBool32 alphaToCoverageEnable,
	const VkBool32 alphaToOneEnable
)
{
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = sampleShadingEnable;
	multisampling.rasterizationSamples = rasterizationSamples;
	multisampling.minSampleShading = minSampleShading;
	multisampling.pSampleMask = sampleMask;
	multisampling.alphaToCoverageEnable = alphaToCoverageEnable;
	multisampling.alphaToOneEnable = alphaToOneEnable;
	return multisampling;
}

VkPipelineColorBlendAttachmentState getPipelineColorBlendAttachmentState(
	const VkColorComponentFlags colorWriteMask,
	const VkBool32 blendEnable,
	const VkBlendFactor srcColorBlendFactor,
	const VkBlendFactor dstColorBlendFactor,
	const VkBlendOp colorBlendOp,
	const VkBlendFactor srcAlphaBlendFactor,
	const VkBlendFactor dstAlphaBlendFactor,
	const VkBlendOp alphaBlendOp
)
{
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = colorWriteMask;
	colorBlendAttachment.blendEnable = blendEnable;
	colorBlendAttachment.srcColorBlendFactor = srcColorBlendFactor;
	colorBlendAttachment.dstColorBlendFactor = dstColorBlendFactor;
	colorBlendAttachment.colorBlendOp = colorBlendOp;
	colorBlendAttachment.srcAlphaBlendFactor = srcAlphaBlendFactor;
	colorBlendAttachment.dstAlphaBlendFactor = dstAlphaBlendFactor;
	colorBlendAttachment.alphaBlendOp = alphaBlendOp;
	return colorBlendAttachment;
}

VkPipelineColorBlendStateCreateInfo getPipelineColorBlendStateCreateInfo(
	const VkBool32 logicOpEnable,
	const VkLogicOp logicOp,
	const uint32_t attachmentCount,
	const VkPipelineColorBlendAttachmentState* colorBlendAttachment,
	const float blendConstant0,
	const float blendConstant1,
	const float blendConstant2,
	const float blendConstant3
)
{
	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = colorBlendAttachment;
	colorBlending.blendConstants[0] = blendConstant0;
	colorBlending.blendConstants[1] = blendConstant1;
	colorBlending.blendConstants[2] = blendConstant2;
	colorBlending.blendConstants[3] = blendConstant3;
	return colorBlending;
}

VkPipelineDepthStencilStateCreateInfo getPipelineDepthStencilStateCreateInfo(
	const VkBool32 depthTestEnable,
	const VkBool32 depthWriteEnable,
	const VkCompareOp depthCompareOp,
	const VkBool32 depthBoundsTestEnable,
	const float minDepthBounds,
	const float maxDepthBounds,
	const VkBool32 stencilTestEnable,
	const VkStencilOpState front,
	const VkStencilOpState back
)
{
	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType =
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = depthTestEnable;
	depthStencil.depthWriteEnable = depthWriteEnable;
	depthStencil.depthCompareOp = depthCompareOp;
	depthStencil.depthBoundsTestEnable = depthBoundsTestEnable;
	depthStencil.minDepthBounds = minDepthBounds; // Optional
	depthStencil.maxDepthBounds = maxDepthBounds; // Optional
	depthStencil.stencilTestEnable = stencilTestEnable;
	depthStencil.front = front; // Optional
	depthStencil.back = back; // Optional
	return depthStencil;
}