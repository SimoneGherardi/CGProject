#pragma once
#include "defines.h"
#include "graphics_pipeline.h"
#include "graphics_pipeline_layout.h"
#include "graphics_pipeline_states.h"
#include "viewport.h"
#include "ioutils.h"
#include "shader_module.h"
#include "ThreeDimensionalDataTypes.h"

extern void initializeStdPipeline(
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
);