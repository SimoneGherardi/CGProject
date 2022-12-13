#pragma once
#include "defines.h"

typedef std::vector<VkFramebuffer> Framebuffers;

extern void initializeFrameBuffers(const VkDevice device, const VkExtent2D extent, const VkRenderPass renderPass, const std::vector<VkImageView> swapChainImages, Framebuffers framebuffers);
extern void cleanupFrameBuffer(const VkDevice device, const Framebuffers framebuffers);