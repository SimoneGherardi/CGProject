#pragma once
#include "defines.h"

extern VkAttachmentDescription getAttachmentDescription(
    const VkFormat format,
    const VkSampleCountFlagBits samples,
    const VkAttachmentLoadOp loadOp,
    const VkAttachmentStoreOp storeOp,
    const VkAttachmentLoadOp stencilLoadOp,
    const VkAttachmentStoreOp stencilStoreOp,
    const VkImageLayout initialLayout,
    const VkImageLayout finalLayout
);

extern VkAttachmentReference getAttachmentReference(
    const uint32_t attachment,
    const VkImageLayout layout
);

extern VkSubpassDescription getSubpassDescription(
    const VkPipelineBindPoint pipelineBindPoint,
    const uint32_t colorAttachmentCount,
    const VkAttachmentReference* colorAttachmentRef,
    const VkAttachmentReference* depthAttachmentRef,
    const VkAttachmentReference* colorAttachmentResolveRef
);

extern VkSubpassDependency getSubpassDependency(
    const uint32_t srcSubpass,
    const uint32_t dstSubpass,
    const VkPipelineStageFlags srcStageMask,
    const VkAccessFlags srcAccessMask,
    const VkPipelineStageFlags dstStageMask,
    const VkAccessFlags dstAccessMask
);

extern void initializeRenderPass(
    const VkPhysicalDevice physicalDevice,
    const VkDevice device,
    const VkFormat format,
    const VkSampleCountFlagBits msaaSamples,
    VkRenderPass* renderPass
);

extern void initializeGUIRenderPass(
    const VkPhysicalDevice physicalDevice,
    const VkDevice device,
    const VkFormat format,
    const VkSampleCountFlagBits msaaSamples,
    VkRenderPass* renderPass
);

extern void cleanupRenderPass(const VkDevice device, VkRenderPass renderPass);

extern VkFormat findDepthFormat(const VkPhysicalDevice physicalDevice);

extern VkFormat findSupportedFormat(
    const VkPhysicalDevice physicalDevice,
    const std::vector<VkFormat> candidates,
    const VkImageTiling tiling,
    const VkFormatFeatureFlags features
);