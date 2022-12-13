#pragma once
#include "defines.h"
#include "validation_layers.h"
#include "extensions.h"

struct EngineInfo
{
    std::string engineName;
    uint32_t engineVersion;
};

extern VkApplicationInfo getApplicationInfoApiEngine(const char* applicationName, const uint32_t applicationVersion, const uint32_t apiVersion, const struct EngineInfo engineInfo);
extern VkApplicationInfo getApplicationInfoApi(const char* applicationName, const uint32_t applicationVersion, const uint32_t apiVersion);
extern VkApplicationInfo getApplicationInfo(const char* applicationName, const uint32_t applicationVersion);

extern VkInstanceCreateInfo getInstanceCreateInfo(
    const VkApplicationInfo& applicationInfo,
    const ValidationLayers& validationLayers,
    const Extensions& extensions,
    VkDebugUtilsMessengerCreateInfoEXT* debugMessengerCreateInfo
);

extern void initializeInstance(const VkInstanceCreateInfo& createInfo, VkInstance* instance);