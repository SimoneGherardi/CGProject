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
#include "swap_chain_info.h"
#include "image_views.h"
#include "render_passes.h"
#include "command_pool.h"
#include "render_target.h"
#include "framebuffers.h"

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
	SwapchainInfo _Swapchain;
	RenderTarget _ColorRenderTarget;
	RenderTarget _DepthRenderTarget;

	VkRenderPass _RenderPass;

	VkCommandPool _CommandPool;

	std::vector<VkFramebuffer> _SwapChainFramebuffers;

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

	void _InitializeRenderPass() {
		TRACESTART;
		initializeRenderPass(
			_PhysicalDevice,
			_Device,
			_Swapchain.GetFormat(),
			VK_SAMPLE_COUNT_2_BIT,
			&_RenderPass
		);
		_CleanupStack.push([=]() {
			LOGDBG("cleaning up render pass");
			cleanupRenderPass(_Device, _RenderPass);
		});
		TRACEEND;
	}

	void _InitializeCommandPool() {
		TRACESTART;
		auto queueFamilyIndices = findQueueFamilies(_PhysicalDevice, _Surface);
		initializeCommandPool(
			_Device,
			queueFamilyIndices,
			&_CommandPool
		);
		_CleanupStack.push([=]() {
			LOGDBG("cleaning up command pool");
			cleanupCommandPool(_Device, _CommandPool);
		});
		TRACEEND;
	}

	void _InitializeRenderTargets()
	{
		TRACESTART;
		_ColorRenderTarget.Initialize(
			_PhysicalDevice,
			_Device,
			_Swapchain.GetExtent(),
			_Swapchain.GetFormat(),
			1,
			VK_SAMPLE_COUNT_2_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT
		);
		_DepthRenderTarget.Initialize(
			_PhysicalDevice,
			_Device,
			_Swapchain.GetExtent(),
			findDepthFormat(_PhysicalDevice),
			1,
			VK_SAMPLE_COUNT_2_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT
		);
		_CleanupStack.push([=]() {
			LOGDBG("cleaning up render targets");
			_DepthRenderTarget.Cleanup();
			_ColorRenderTarget.Cleanup();
		});
		TRACEEND;
	}

	void _InitializeFrameBuffer() {
		TRACESTART;
		_SwapChainFramebuffers.resize(_Swapchain.GetImagesViews().size());
		for (size_t i = 0; i < _Swapchain.GetImagesViews().size(); i++) {
			std::array<VkImageView, 3> attachments = {
				_ColorRenderTarget.GetImageView(),
				_DepthRenderTarget.GetImageView(),
				_Swapchain.GetImagesViews()[i]
			};

			initializeFrameBuffer(
				_Device,
				static_cast<uint32_t>(attachments.size()),
				attachments.data(),
				_RenderPass,
				_Swapchain.GetExtent(),
				&_SwapChainFramebuffers[i]
			);

			_CleanupStack.push([=]() {
				LOGDBG("cleaning up framebuffer");
				cleanupFrameBuffer(
					_Device,
					_SwapChainFramebuffers[i]
				);
			});
		}
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
		_InitializeRenderPass();
		// descriptor set layouts
		// pipeline
		_InitializeCommandPool();
		_InitializeRenderTargets();
		_InitializeFrameBuffer();
		TRACEEND;
	}

	void Cleanup()
	{
		TRACESTART;
		_CleanupStack.flush();
		TRACEEND;
	}
};