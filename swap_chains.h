#pragma once
#include "defines.h"
#include "swap_chain_support.h"
#include "queue_families.h"

extern void initializeSwapChain(
    const int width,
    const int height,
    const VkPhysicalDevice physicalDevice,
    const VkDevice device,
    const VkSurfaceKHR surface,
    VkSwapchainKHR* swapChain,
    std::vector<VkImage>* swapChainImages,
    VkFormat* swapChainImageFormat,
    VkExtent2D* swapChainExtent
);
extern VkSurfaceFormatKHR getSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats);
extern VkPresentModeKHR getSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
extern VkExtent2D getSwapExtent(const VkSurfaceCapabilitiesKHR capabilities, const int width, const int height);

extern void cleanupSwapChain(
    const VkDevice device,
    const VkSwapchainKHR swapChain
);

class Swapchain
{
private:
    VkDevice _Device;
    VkSwapchainKHR _Swapchain;
    std::vector<VkImage> _Images;
    VkFormat _Format;
    VkExtent2D _Extent;

public:
    void Initialize(
        const int width,
        const int height,
        const VkPhysicalDevice physicalDevice,
        const VkDevice device,
        const VkSurfaceKHR surface
    );
    void Cleanup();
    VkSwapchainKHR GetSwapchain();
    std::vector<VkImage> GetImages();
    VkFormat GetFormat();
    VkExtent2D GetExtent();
};