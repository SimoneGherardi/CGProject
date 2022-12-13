#include "physical_device.h"

std::vector<VkPhysicalDevice> getPhysicalDevices(const VkInstance& instance)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    return devices;
}

VkPhysicalDevice getOptimalPhysicalDevice(const std::vector<VkPhysicalDevice>& devices, const VkSurfaceKHR& surface, const Extensions& deviceExtensions)
{
    VkPhysicalDevice physicalDevice = {};
    uint32_t deviceCount = devices.size();

    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::multimap<float, VkPhysicalDevice> candidates;

    for (const auto& device : devices)
    {
        float score = getPhysicalDeviceSuitability(device, surface, deviceExtensions);
        candidates.insert(std::make_pair(score, device));
    }

    if (candidates.rbegin()->first > 0) {
        physicalDevice = candidates.rbegin()->second;
    }
    else
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    return physicalDevice;
}

float getPhysicalDeviceSuitability(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, const Extensions& deviceExtensions)
{
    float score = 0;
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }

    score += deviceProperties.limits.maxImageDimension2D;

    if (!isDeviceSuitable(device, surface, deviceExtensions))
    {
        return 0;
    }
    if (!deviceFeatures.geometryShader) {
        return 0;
    }

    return score;
}

bool isDeviceSuitable(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, const Extensions& deviceExtensions)
{
    QueueFamilyIndices indices = findQueueFamilies(device, surface);

    bool extensionsSupported = checkPhysicalDeviceExtensionSupport(device, deviceExtensions);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() &&
            !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return indices.isComplete() && extensionsSupported && swapChainAdequate &&
        supportedFeatures.samplerAnisotropy;
}



bool checkPhysicalDeviceExtensionSupport(const VkPhysicalDevice& device, const Extensions& deviceExtensions)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr,
        &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr,
        &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(),
        deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}