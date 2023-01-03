#pragma once
#include "defines.h"
#include "swap_chain_support.h"
#include "queue_families.h"

extern void initializeSwapChain(
    GLFWwindow* window,
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
extern VkExtent2D getSwapExtent(const VkSurfaceCapabilitiesKHR capabilities, GLFWwindow* window);