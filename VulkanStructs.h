#pragma once
#include "defines.h"
#include "VertexData.h"

namespace VulkanStructs {
	const VkCommandPoolCreateInfo CommandPoolCreateInfo(const VkCommandPoolResetFlags flags = 0);
	const VkCommandBufferAllocateInfo CommandBufferAllocateInfo(const VkCommandPool pool, const uint32_t count = 1, const VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	const VkCommandBufferBeginInfo CommandBufferBeginInfo(const VkCommandBufferUsageFlags flags = 0);
	const VkFramebufferCreateInfo FramebufferCreateInfo(const VkRenderPass renderPass, const VkExtent2D extent);
	const VkFenceCreateInfo FenceCreateInfo(const VkFenceCreateFlags flags = 0);
	const VkSemaphoreCreateInfo SemaphoreCreateInfo(const VkSemaphoreCreateFlags flags = 0);
	const VkSubmitInfo SubmitInfo(const VkCommandBuffer* cmd);
	const VkPresentInfoKHR PresentInfo();
	const VkRenderPassBeginInfo RenderpassBeginInfo(const VkRenderPass renderPass, const VkExtent2D windowExtent, const VkFramebuffer framebuffer);
	const VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(const VkShaderStageFlagBits stage, const VkShaderModule shaderModule);
	const VkPipelineVertexInputStateCreateInfo VertexInputStateCreateInfo();
	const VkPipelineInputAssemblyStateCreateInfo InputAssemblyCreateInfo(const VkPrimitiveTopology topology);
	const VkPipelineRasterizationStateCreateInfo RasterizationStateCreateInfo(const VkPolygonMode polygonMode);
	const VkPipelineMultisampleStateCreateInfo MultisamplingStateCreateInfo();
	const VkPipelineColorBlendAttachmentState ColorBlendAttachmentState();
	const VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo();
	const VkImageCreateInfo ImageCreateInfo(const VkFormat format, const VkImageUsageFlags usageFlags, const VkExtent3D extent);
	const VkImageViewCreateInfo ImageviewCreateInfo(const VkFormat format, const VkImage image, const VkImageAspectFlags aspectFlags);
	const VkPipelineDepthStencilStateCreateInfo DepthStencilCreateInfo(const bool bDepthTest, const bool bDepthWrite, const VkCompareOp compareOp);
	const VkDescriptorSetLayoutBinding DescriptorsetLayoutBinding(const VkDescriptorType type, const VkShaderStageFlags stageFlags, const uint32_t binding);
	const VkWriteDescriptorSet WriteDescriptorBuffer(const VkDescriptorType type, const VkDescriptorSet dstSet, const VkDescriptorBufferInfo* bufferInfo, const uint32_t binding);
	const VkWriteDescriptorSet WriteDescriptorImage(const VkDescriptorType type, const VkDescriptorSet dstSet, const VkDescriptorImageInfo* imageInfo, const uint32_t binding);
	const VkSamplerCreateInfo SamplerCreateInfo(const VkFilter filters, const VkSamplerAddressMode samplerAdressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);
	const VkBufferMemoryBarrier BufferBarrier(const VkBuffer buffer, const uint32_t queue);
	const VkImageMemoryBarrier ImageBarrier(const VkImage image, const VkAccessFlags srcAccessMask, const VkAccessFlags dstAccessMask, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkImageAspectFlags aspectMask);
	const VkVertexInputBindingDescription VertexInputBindingDescription(const uint32_t binding, const uint32_t size, const VkVertexInputRate rate);
	const std::vector<VkVertexInputAttributeDescription> VertexInputAttributeDescription(const uint32_t binding);
}