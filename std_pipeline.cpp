#include "std_pipeline.h"
#include "VulkanStructs.h"

void initializeStdPipeline(
	const VkDevice device,
	const std::string vertexShaderPath,
	const std::string fragmentShaderPath,
	const VkExtent2D extent,
	const VkSampleCountFlagBits msaaSamples,
	const VkCompareOp compareOp,
	//const VkDescriptorSetLayout* descriptorSetLayout,
	const VkPolygonMode polygonMode,
	const VkCullModeFlagBits cullBits,
	const bool transparency,
	const VkPipelineLayout pipelineLayout,
	const VkRenderPass renderPass,
	// VertexDescriptor* vertexDescriptor,
	VkPipeline* pipeline
)
{
	auto vertexShaderCode = readFile(vertexShaderPath);
	auto fragmentShaderCode = readFile(fragmentShaderPath);

	auto vertexShader = initializeShaderModule(device, vertexShaderCode);
	auto fragmentShader = initializeShaderModule(device, fragmentShaderCode);

	auto vertexShaderStageInfo = getPipelineShaderStageCreateInfo(vertexShader, VK_SHADER_STAGE_VERTEX_BIT);
	auto fragmentShaderStageInfo = getPipelineShaderStageCreateInfo(fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT);

	VkPipelineShaderStageCreateInfo shaderStages[] = {
		vertexShaderStageInfo, fragmentShaderStageInfo
	};

	auto bindingDescription = VulkanStructs::VertexInputBindingDescription(
		0,
		sizeof(VertexData),
		VK_VERTEX_INPUT_RATE_VERTEX
	);
	
	auto attributeDescriptions = VulkanStructs::VertexInputAttributeDescription(0);

	auto vertexInputInfo = getPipelineVertexInputStateCreateInfo(
		1, &bindingDescription,
		static_cast<uint32_t>(attributeDescriptions.size()),
		attributeDescriptions.data()
	);

	auto inputAssembly = getPipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);

	auto viewport = getViewport(0, 0, extent, 0, 1);
	auto scissor = getScissor(0, 0, extent);

	auto viewportState = getPipelineViewportStateCreateInfo(1, &viewport, 1, &scissor);

	auto rasterizer = getPipelineRasterizationStateCreateInfo(
		VK_FALSE, VK_FALSE, polygonMode, 1, cullBits, VK_FRONT_FACE_COUNTER_CLOCKWISE
	);

	auto multisampling = getPipelineMultisampleStateCreateInfo(
		VK_TRUE, msaaSamples, 1, nullptr, VK_FALSE, VK_FALSE
	);

	auto colorComponents = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	auto blendEnable = transparency ? VK_TRUE : VK_FALSE;
	auto srcColorBlendFactor = transparency ? VK_BLEND_FACTOR_SRC_ALPHA : VK_BLEND_FACTOR_ONE;
	auto dstColorBlendFactor = transparency ? VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA : VK_BLEND_FACTOR_ZERO;
	auto colorBlendAttachment = getPipelineColorBlendAttachmentState(
		colorComponents, blendEnable, srcColorBlendFactor, dstColorBlendFactor,
		VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD
	);

	auto colorBlending = getPipelineColorBlendStateCreateInfo(
		VK_FALSE, VK_LOGIC_OP_COPY, 1, &colorBlendAttachment,
		0, 0, 0, 0
	);

	auto depthStencil = getPipelineDepthStencilStateCreateInfo(
		VK_TRUE, VK_TRUE, compareOp, VK_FALSE, 0, 1, VK_FALSE, {}, {}
	);

	//initializeGraphicsPipelineLayout(
	//	device, 1, descriptorSetLayout, 0, nullptr, pipelineLayout
	//);

	initializeGraphicsPipeline(
		device, 2, shaderStages,
		&vertexInputInfo,
		&inputAssembly,
		&viewportState,
		&rasterizer,
		&multisampling,
		&depthStencil,
		&colorBlending,
		nullptr,
		pipelineLayout,
		renderPass,
		pipeline
	);

	cleanupShaderModule(device, fragmentShader);
	cleanupShaderModule(device, vertexShader);
}