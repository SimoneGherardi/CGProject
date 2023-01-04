#pragma once
#include "framebuffers.h"

void initializeFrameBuffer(
	const VkDevice device,
	const uint32_t attachmentCount,
	const VkImageView* attachments,
	const VkRenderPass renderPass,
	const VkExtent2D extent,
	VkFramebuffer* frameBuffer
)
{
	VkFramebufferCreateInfo  framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = attachmentCount;
	framebufferInfo.pAttachments = attachments;
	framebufferInfo.width = extent.width;
	framebufferInfo.height = extent.height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, frameBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create framebuffer!");
	}
}

void cleanupFrameBuffer(
	const VkDevice device,
	const VkFramebuffer framebuffer
)
{
	vkDestroyFramebuffer(device, framebuffer, nullptr);
}