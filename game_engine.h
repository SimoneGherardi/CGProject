#pragma once
#include "defines.h"
#include "instance.h"
#include "debug_messenger.h"
#include "extensions.h"
#include "validation_layers.h"
#include "surface.h"
#include "cleanup_stack.h"
#include "physical_device.h"
#include "logical_device.h"
#include "swap_chains.h"

struct WindowSize {
	int Width, Height;
};

class GameEngine
{
private:
	VkInstance _Instance;
	VkDebugUtilsMessengerEXT _DebugMessenger;
	VkSurfaceKHR _Surface;
	VkPhysicalDevice _PhysicalDevice;
	VkDevice _Device;
	VkQueue _GraphicsQueue;
	VkQueue _PresentationQueue;
	Swapchain _Swapchain;
	std::vector<VkImage>* _Images;
	VkFormat* _Format;
	VkExtent2D* _Extent;

	CleanupStack _CleanupStack;

	const std::vector<const char*> _DeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	#ifdef __APPLE__
		,"VK_KHR_portability_subset"
	#endif
	};

	#ifdef ENABLE_VALIDATION_LAYERS
	const std::vector<const char*> _ValidationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
	#else
	const std::vector<const char*> _ValidationLayers = {};
	#endif

	void _InitializeInstance(const char* title)
	{
		TRACESTART;
		auto debugCreateInfo = getDebugMessengerCreateInfo(
			DEBUG_CALLBACK,
			DEBUG_MESSENGER_SEVERITY_MASK(1, 1, 1, 1),
			DEBUG_MESSENGER_TYPE_MASK(1, 1, 1),
			nullptr
		);
		auto requiredExtensions = getRequiredExtensions();
		auto appInfo = getApplicationInfo(title, VK_MAKE_API_VERSION(0, 1, 0, 0));
		auto createInfo = getInstanceCreateInfo(
			&_ValidationLayers, &requiredExtensions, &appInfo, &debugCreateInfo
		);
		initializeInstance(&createInfo, &_Instance);
		_CleanupStack.push([=]() {
			LOGDBG("cleaning up instance");
			cleanupInstance(_Instance);
		});
		TRACEEND;
	}

	void _InitializeDebugMessenger()
	{
		TRACESTART;
		auto createInfo = getDebugMessengerCreateInfo(
			DEBUG_CALLBACK,
			DEBUG_MESSENGER_SEVERITY_MASK(1, 1, 1, 1),
			DEBUG_MESSENGER_TYPE_MASK(1, 1, 1),
			nullptr
		);
		initializeDebugMessenger(_Instance, &createInfo, nullptr, &_DebugMessenger);
		_CleanupStack.push([=]() {
			LOGDBG("cleaning up debug messenger");
			cleanupDebugMessenger(_Instance, _DebugMessenger, nullptr);
		});
		TRACEEND;
	}

	void _InitializeSurface(SurfaceFactory* factory)
	{
		TRACESTART;
		initializeSurface(_Instance, &_Surface, factory);
		_CleanupStack.push([=]() {
			LOGDBG("cleaning up surface");
			cleanupSurface(_Instance, _Surface);
		});
		TRACEEND;
	}

	void _InitializePhysicalDevice() {
		TRACESTART;
		auto devices = getPhysicalDevices(_Instance);
		_PhysicalDevice = getOptimalPhysicalDevice(devices, _Surface, _DeviceExtensions);
		TRACEEND;
	}

	void _InitializeLogicalDevice() {
		TRACESTART;
		initializeLogicalDevice(
			_PhysicalDevice,
			_Surface,
			_ValidationLayers,
			_DeviceExtensions,
			&_Device,
			&_GraphicsQueue,
			&_PresentationQueue
		);
		_CleanupStack.push([=]() {
			LOGDBG("cleaning up logical device");
			cleanupLogicalDevice(_Device);
		});
		TRACEEND;
	}

	void _InitializeSwapchain(WindowSize windowSize) {
		TRACESTART;
		_Swapchain.Initialize(
			windowSize.Width, windowSize.Height,
			_PhysicalDevice, _Device, _Surface
		);
		_CleanupStack.push([=]() {
			LOGDBG("cleaning up logical device");
			_Swapchain.Cleanup();
		});
		TRACEEND;
	}

	// Initialize Swapchain details

	void _InitializeSwapchainDetails() {
		TRACESTART;
		*_Images = _Swapchain.GetImages();
		*_Format = _Swapchain.GetFormat();
		*_Extent = _Swapchain.GetExtent();
		TRACEEND;
	}




public:
	void Initialize(const char* title, SurfaceFactory* factory, WindowSize windowSize)
	{
		TRACESTART;
		_InitializeInstance(title);
		#ifdef ENABLE_VALIDATION_LAYERS
		_InitializeDebugMessenger();
		#endif
		_InitializeSurface(factory);
		_InitializePhysicalDevice();
		_InitializeLogicalDevice();
		_InitializeSwapchain(windowSize);
		_InitializeSwapchainDetails();
		TRACEEND;
	}

	void Cleanup()
	{
		TRACESTART;
		_CleanupStack.flush();
		TRACEEND;
	}
};