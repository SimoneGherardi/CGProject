#pragma once
#include "defines.h"

extern void initializeFrameBuffer(
	const VkDevice device,
	const uint32_t attachmentCount,
	const VkImageView* attachments,
	const VkRenderPass renderPass,
	const VkExtent2D extent,
	VkFramebuffer* frameBuffer
);

extern void cleanupFrameBuffer(
	const VkDevice device,
	const VkFramebuffer framebuffer
);