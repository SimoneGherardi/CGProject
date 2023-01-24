#include "image_views.h"

void initializeImageViews(
    const VkDevice device,
    const std::vector<VkImage> swapChainImages,
    const VkFormat swapChainImageFormat,
    std::vector<VkImageView>* swapChainImageViews
)
{
    swapChainImageViews->resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++)
    {
        (*swapChainImageViews)[i] = initializeImageView(
            device,
            swapChainImages[i],
            swapChainImageFormat,
            VK_IMAGE_ASPECT_COLOR_BIT,
            1,
            VK_IMAGE_VIEW_TYPE_2D,
            1
        );
    }
}

VkImageView initializeImageView(
    const VkDevice device,
    const VkImage image,
    const VkFormat format,
    const VkImageAspectFlags aspectFlags,
    const uint32_t mipLevels,
    const VkImageViewType type,
    const int layerCount
)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = type;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = layerCount;
    VkImageView imageView;

    VkResult result = vkCreateImageView(device, &viewInfo, nullptr,
        &imageView);
    if (result != VK_SUCCESS) {
        PrintVkError(result);
        throw std::runtime_error("failed to create image view!");
    }
    return imageView;
}

void cleanupImageView(
    const VkDevice device,
    const VkImageView imageView
)
{
    vkDestroyImageView(device, imageView, nullptr);
}