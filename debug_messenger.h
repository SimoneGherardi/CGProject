#pragma once
#include "defines.h"

#define DEBUG_MESSENGER_SEVERITY_MASK(error, warning, info, verbose) (	\
	error * VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |				\
	warning * VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |			\
	info * VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |				\
	verbose * VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT			\
)
#define DEBUG_MESSENGER_TYPE_MASK(general, validation, performance) (	\
	general * VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |				\
	validation * VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |		\
	performance * VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT		\
)

// extern inline VkDebugUtilsMessageSeverityFlagsEXT getDebugMessengerSeverityMask(const bool error, const bool warning, const bool info, const bool verbose);
// extern inline VkDebugUtilsMessageTypeFlagsEXT getDebugMessengerTypeMask(const bool general, const bool validation, const bool performance);

extern VkDebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo(
	PFN_vkDebugUtilsMessengerCallbackEXT debugCallback,
	const VkDebugUtilsMessageSeverityFlagsEXT severityMask,
	const VkDebugUtilsMessageTypeFlagsEXT typeMask,
	void* userData
);

extern void initializeDebugMessenger(
	const VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger
);

extern void cleanupDebugMessenger(const VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);