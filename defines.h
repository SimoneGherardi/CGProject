#pragma once

#ifdef NDEBUG
#define LOGDBG(x)
#define TRACESTART
#define TRACEEND
#else
#define ENABLE_VALIDATION_LAYERS
#define LOGDBG(x) std::cout << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] " << x << std::endl
#define TRACESTART LOGDBG("TRACE START")
#define TRACEEND LOGDBG("TRACE END")
#endif

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp

#include <string>
#include <stdint.h>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <map>
#include <optional>
#include <set>
#include <array>
#include <unordered_map>
#include <fstream>
#include <functional>
#include <stack>

struct ErrorCode_t {
    VkResult resultCode;
    std::string meaning;
};

struct VulkanContext_T {
    VkInstance Instance;
    VkPhysicalDevice PhysicalDevice;
    VkDevice Device;
    VkQueue GraphicsQueue;
    VkQueue PresentationQueue;
    VkSurfaceKHR Surface;
};

typedef VulkanContext_T* VulkanContext;

extern VKAPI_ATTR VkBool32 VKAPI_CALL DEBUG_CALLBACK(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData
);

extern void PrintVkError(VkResult result);

extern void CheckVkResult(VkResult result);