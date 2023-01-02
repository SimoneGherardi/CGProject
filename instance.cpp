#include "instance.h"

VkApplicationInfo getApplicationInfoApiEngine(const char* applicationName, const uint32_t applicationVersion, const uint32_t apiVersion, const struct EngineInfo engineInfo)
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

VkApplicationInfo getApplicationInfoApi(const char* applicationName, const uint32_t applicationVersion, const uint32_t apiVersion)
{
    return getApplicationInfoApiEngine(applicationName, applicationVersion, apiVersion, { "Unknown Engine", VK_MAKE_API_VERSION(0,0,0,0) });
}

VkApplicationInfo getApplicationInfo(const char* applicationName, const uint32_t applicationVersion)
{
    return getApplicationInfoApi(applicationName, applicationVersion, VK_API_VERSION_1_0);
}

VkInstanceCreateInfo getInstanceCreateInfo(
    const VkApplicationInfo applicationInfo,
    const ValidationLayers validationLayers,
    const Extensions extensions,
    VkDebugUtilsMessengerCreateInfoEXT* debugMessengerCreateInfo
)
{
#ifdef ENABLE_VALIDATION_LAYERS
    if (!checkValidationLayersSupport(validationLayers))
        throw std::runtime_error("validation layers requested, but not available!");
#endif

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &applicationInfo;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef ENABLE_VALIDATION_LAYERS
        createInfo.enabledLayerCount = validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();
        createInfo.pNext = debugMessengerCreateInfo;
#else
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
#endif

    return createInfo;
}

void initializeInstance(const VkInstanceCreateInfo instanceCreateInfo, VkInstance* instance)
{
    auto result = vkCreateInstance(&instanceCreateInfo, nullptr, instance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}









/*
void createInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}
*/