#pragma once

#ifdef NDEBUG
#else
#define ENABLE_VALIDATION_LAYERS
#endif

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

extern VKAPI_ATTR VkBool32 VKAPI_CALL DEBUG_CALLBACK(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData
);

extern void PrintVkError(VkResult result);

extern void CheckVkResult(VkResult result);