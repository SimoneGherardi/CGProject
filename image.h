#pragma once
#include "defines.h"

extern void initializeImage(
    const VkPhysicalDevice physicalDevice,
    const VkDevice device,
    const uint32_t width,
    const uint32_t height,
    const uint32_t mipLevels,
    const VkSampleCountFlagBits numSamples,
    const VkFormat format,
    const VkImageTiling tiling,
    const VkImageUsageFlags usage,
    const VkMemoryPropertyFlags properties,
    VkImage* image,
    VkDeviceMemory* imageMemory
);

extern void cleanupImage(
    const VkDevice device,
    const VkImage image,
    const VkDeviceMemory memory
);

extern uint32_t findMemoryType(
    const VkPhysicalDevice physicalDevice,
    const uint32_t typeFilter,
    const VkMemoryPropertyFlags properties
);