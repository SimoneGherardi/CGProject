#include "image.h"

void initializeImage(
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
)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = numSamples;
    imageInfo.flags = 0; // Optional

    VkResult result = vkCreateImage(device, &imageInfo, nullptr, image);
    CheckVkResult(result);

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, *image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(
        physicalDevice,
        memRequirements.memoryTypeBits,
        properties
    );
    CheckVkResult(vkAllocateMemory(device, &allocInfo, nullptr, imageMemory));
    vkBindImageMemory(device, *image, *imageMemory, 0);
}

void cleanupImage(
    const VkDevice device,
    const VkImage image,
    const VkDeviceMemory memory
)
{
    vkDestroyImage(
        device,
        image,
        nullptr
    );
    vkFreeMemory(device, memory, nullptr);
}

uint32_t findMemoryType(
    const VkPhysicalDevice physicalDevice,
    const uint32_t typeFilter,
    const VkMemoryPropertyFlags properties
)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) ==
            properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}