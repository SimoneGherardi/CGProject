#pragma once
#include "defines.h"

extern void initializeImageViews(
    const VkDevice& device,
    const std::vector<VkImage>& swapChainImages,
    const VkFormat& swapChainImageFormat,
    std::vector<VkImageView>* swapChainImageViews
);

extern VkImageView initializeImageView(
    const VkDevice& device,
    const VkImage& image,
    const VkFormat format,
    const VkImageAspectFlags aspectFlags,
    const uint32_t mipLevels,
    const VkImageViewType type,
    const int layerCount
);