#include "swap_chain_info.h"

void SwapchainInfo::Initialize(
    const int width,
    const int height,
    const VkPhysicalDevice physicalDevice,
    const VkDevice device,
    const VkSurfaceKHR surface
)
{
    _Device = device;
    initializeSwapChain(
        width, height,
        physicalDevice,
        _Device,
        surface,
        &_Swapchain,
        &_Images,
        &_Format,
        &_Extent
    );
    initializeImageViews(
        _Device,
        GetImages(),
        GetFormat(),
        &_ImageViews
    );
}

void SwapchainInfo::Cleanup()
{
    for (VkImageView view : _ImageViews)
    {
        cleanupImageView(_Device, view);
    }
    cleanupSwapChain(_Device, _Swapchain);
}

VkSwapchainKHR SwapchainInfo::GetSwapchain() const
{
    return _Swapchain;
}

std::vector<VkImage> SwapchainInfo::GetImages() const
{
    return _Images;
}

std::vector<VkImageView> SwapchainInfo::GetImagesViews() const
{
    return _ImageViews;
}

VkFormat SwapchainInfo::GetFormat() const
{
    return _Format;
}

VkExtent2D SwapchainInfo::GetExtent() const
{
    return _Extent;
}
