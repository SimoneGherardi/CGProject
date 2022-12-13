#include "debug_messenger.h"

/*
inline VkDebugUtilsMessageSeverityFlagsEXT getDebugMessengerSeverityMask(const bool error, const bool warning, const bool info, const bool verbose) {
	return
		error * VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
		warning * VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		info * VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		verbose * VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
}

inline VkDebugUtilsMessageTypeFlagsEXT getDebugMessengerTypeMask(const bool general, const bool validation, const bool performance) {
	return
		general * VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		validation * VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		performance * VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
}
*/

VkDebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo(
	PFN_vkDebugUtilsMessengerCallbackEXT debugCallback,
	const VkDebugUtilsMessageSeverityFlagsEXT severityMask,
	const VkDebugUtilsMessageTypeFlagsEXT typeMask,
	void* userData
)
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = severityMask;
	createInfo.messageType = typeMask;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = userData;

	return createInfo;
}

void initializeDebugMessenger(
	const VkInstance& instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger
)
{
	VkResult result;
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		result = func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		result = VK_ERROR_EXTENSION_NOT_PRESENT;
	}
	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

void cleanupDebugMessenger(const VkInstance& instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}