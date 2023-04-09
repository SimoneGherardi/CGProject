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
// #include "Allocator.h"
#include "FrameData.h"
#include "descriptor_set_layout.h"
#include "descriptor_pool.h"
#include "descriptor_set.h"
#include "std_pipeline.h"
#include "SyncStructures.h"
#include "CommandBuffer.h"
#include "tests.h"
#include "RenderContext.h"
#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"
#include "MemoryTransferer.h"
#include "ImmediateCommandBuffer.h"
#include "camera.h"

#define FRAME_OVERLAP 3
constexpr auto HOST_VISIBLE_MEMORY_SIZE = (4 * 1024 * 1024);
constexpr auto DEVICE_LOCAL_MEMORY_SIZE = (4 * 1024 * 1024);
constexpr auto MAX_COMMANDS = 2048;
constexpr auto MAX_INSTANCES = 4096;

struct WindowSize {
	int Width, Height;
};

class RenderingEngine
{
private:
	WindowSize _WindowSize;
	VkDebugUtilsMessengerEXT _DebugMessenger;

	SwapchainInfo _Swapchain;
	RenderTarget _ColorRenderTarget;
	RenderTarget _DepthRenderTarget;

	VulkanContext_T _Context;

	VkRenderPass _RenderPass;

	VkPipelineLayout _PipelineLayout;
	VkPipeline _Pipeline;

	Fence* _RenderFence;
	Semaphore* _PresentSemaphore;
	Semaphore* _RenderSemaphore;

	VkCommandPool _CommandPool;
	CommandBuffer* _MainCommandBuffer;

	std::vector<VkFramebuffer> _SwapChainFramebuffers;

	std::array<FrameData, FRAME_OVERLAP> _FrameData = {};
	uint8_t _CurrentFrameIndex = 0;
	// HostLocalAllocator* _FrameDataAllocator;
	VkDescriptorPool _FrameDataDescriptorPool;
	VkDescriptorSetLayout _FrameDataDescriptorSetLayout;
	VkDescriptorSetLayout _ObjectDescriptorSetLayout;

	VkDescriptorPool _GuiDescriptorPool;
	ImmediateCommandBuffer* _GuiCommandBuffer;

	DeviceMemory* _HostVisibleMemory;
	DeviceMemory* _DeviceLocalMemory;

	CleanupStack _CleanupStack;

	const std::vector<const char*> _DeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
	VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME
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

	void _InitializeInstance(const char* title);
	void _InitializeDebugMessenger();
	void _InitializeSurface(SurfaceFactory* factory);
	void _InitializePhysicalDevice();
	void _InitializeLogicalDevice();
	void _InitializeSwapchain(WindowSize windowSize);
	void _InitializeRenderPass();
	void _InitializeAllocators();
	void _InitializeDescriptorSet();
	void _InitializePipeline();
	void _InitializeSyncStructures();
	void _InitializeCommandPool();
	void _InitializeCommandBuffers();
	void _InitializeRenderTargets();
	void _InitializeFrameBuffer();
	void _InitializeModels();
	void _InitializeGui();

	RenderingEngine() = default;
public:
	FrameData* GetCurrentFrameData();
	void Initialize(const char* title, SurfaceFactory* factory, WindowSize windowSize);
	void Render(float delta, CameraInfos* camera);
	void Cleanup();

	static RenderingEngine& GetInstance();
};