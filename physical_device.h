#pragma once
#include "defines.h"
#include "queue_families.h"
#include "extensions.h"
#include "swap_chain_support.h"

extern std::vector<VkPhysicalDevice> getPhysicalDevices(const VkInstance& instance);
extern VkPhysicalDevice getOptimalPhysicalDevice(const std::vector<VkPhysicalDevice>& devices, const VkSurfaceKHR& surface, const Extensions& deviceExtensions);
extern float getPhysicalDeviceSuitability(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, const Extensions& deviceExtensions);
extern bool isDeviceSuitable(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, const Extensions& deviceExtensions);
extern bool checkPhysicalDeviceExtensionSupport(const VkPhysicalDevice& device, const Extensions& deviceExtensions);