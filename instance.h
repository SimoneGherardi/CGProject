#pragma once
#include "defines.h"
#include "validation_layers.h"
#include "extensions.h"

struct EngineInfo
{
    std::string engineName;
    uint32_t engineVersion;
};

extern VkApplicationInfo getApplicationInfo(const char* applicationName, const uint32_t applicationVersion, const uint32_t apiVersion, const struct EngineInfo engineInfo);
extern VkApplicationInfo getApplicationInfo(const char* applicationName, const uint32_t applicationVersion, const uint32_t apiVersion);
extern VkApplicationInfo getApplicationInfo(const char* applicationName, const uint32_t applicationVersion);

extern VkInstanceCreateInfo getInstanceCreateInfo(
    const ValidationLayers* validationLayers,
    const Extensions* extensions,
    const VkApplicationInfo* applicationInfo,
    VkDebugUtilsMessengerCreateInfoEXT* debugMessengerCreateInfo
);

extern void initializeInstance(const VkInstanceCreateInfo* createInfo, VkInstance* instance);

extern void cleanupInstance(const VkInstance instance);