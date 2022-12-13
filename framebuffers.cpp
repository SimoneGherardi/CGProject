#pragma once
#include "framebuffers.h"

void initializeFrameBuffers(const VkDevice device, const VkExtent2D extent, const VkRenderPass renderPass, const std::vector<VkImageView> swapChainImages, Framebuffers framebuffers)
{
	const auto size = swapChainImages.size();
	framebuffers.resize(size);
	for (size_t i = 0; i < size; i++)
	{
		const VkImageView attachments[] = {
			swapChainImages[i]
		};

		VkFramebufferCreateInfo  framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;

		VkFramebuffer* bufferPtr = &framebuffers[i];
		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, bufferPtr) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void cleanupFrameBuffer(const VkDevice device, const Framebuffers framebuffers)
{
	for (auto fb : framebuffers)
	{
		vkDestroyFramebuffer(device, fb, nullptr);
	}
}