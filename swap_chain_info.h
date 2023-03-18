#pragma once
#include "defines.h"
#include "swap_chains.h"
#include "image_views.h"


class SwapchainInfo
{
private:
    VkDevice _Device;
    VkSwapchainKHR _Swapchain;
    std::vector<VkImage> _Images;
    VkFormat _Format;
    VkExtent2D _Extent;
    std::vector<VkImageView> _ImageViews;
public:
    void Initialize(
        const int width,
        const int height,
        const VkPhysicalDevice physicalDevice,
        const VkDevice device,
        const VkSurfaceKHR surface
    );
    void Cleanup();
    VkSwapchainKHR GetSwapchain() const;
    std::vector<VkImage> GetImages() const;
    std::vector<VkImageView> GetImagesViews() const;
    VkFormat GetFormat() const;
    VkExtent2D GetExtent() const;
};