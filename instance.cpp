#include "instance.h"

VkApplicationInfo getApplicationInfo(const char* applicationName, const uint32_t applicationVersion, const uint32_t apiVersion, const struct EngineInfo engineInfo)
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = applicationName;
    appInfo.applicationVersion = applicationVersion;
    appInfo.apiVersion = apiVersion;
    appInfo.pEngineName = engineInfo.engineName.c_str();
    appInfo.engineVersion = engineInfo.engineVersion;
    return appInfo;
}

VkApplicationInfo getApplicationInfo(const char* applicationName, const uint32_t applicationVersion, const uint32_t apiVersion)
{
    return getApplicationInfo(applicationName, applicationVersion, apiVersion, { "Unknown Engine", VK_MAKE_API_VERSION(0,0,0,0) });
}

VkApplicationInfo getApplicationInfo(const char* applicationName, const uint32_t applicationVersion)
{
    return getApplicationInfo(applicationName, applicationVersion, VK_API_VERSION_1_0);
}

VkInstanceCreateInfo getInstanceCreateInfo(
    const ValidationLayers* validationLayers,
    const Extensions* extensions,
    const VkApplicationInfo* applicationInfo,
    VkDebugUtilsMessengerCreateInfoEXT* debugMessengerCreateInfo
)
{
    bool validationLayersPresent = validationLayers->size() > 0;

    if (validationLayersPresent)
    {
        if (!checkValidationLayersSupport(*validationLayers))
            throw std::runtime_error("validation layers requested, but not available!");
    }

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = applicationInfo;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions->size());
    createInfo.ppEnabledExtensionNames = extensions->data();


    if (validationLayersPresent)
    {
        createInfo.enabledLayerCount = validationLayers->size();
        createInfo.ppEnabledLayerNames = validationLayers->data();
        createInfo.pNext = debugMessengerCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    return createInfo;
}

void initializeInstance(const VkInstanceCreateInfo* instanceCreateInfo, VkInstance* instance)
{
    auto result = vkCreateInstance(instanceCreateInfo, nullptr, instance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

void cleanupInstance(const VkInstance instance)
{
    vkDestroyInstance(instance, nullptr);
}