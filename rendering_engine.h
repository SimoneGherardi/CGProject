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
#include "Allocator.h"
#include "FrameData.h"
#include "descriptor_set_layout.h"
#include "descriptor_pool.h"
#include "descriptor_set.h"
#include "std_pipeline.h"
#include "SyncStructures.h"
#include "CommandBuffer.h"
#include "tests.h"
#include "RenderContext.h"

#define FRAME_OVERLAP 3

struct WindowSize {
	int Width, Height;
};

class RenderingEngine
{
private:
	WindowSize _WindowSize;
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

	VulkanContext _Context;

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
	HostLocalAllocator* _FrameDataAllocator;
	VkDescriptorPool _FrameDataDescriptorPool;
	VkDescriptorSetLayout _FrameDataDescriptorSetLayout;
	VkDescriptorSetLayout _ObjectDescriptorSetLayout;

	CleanupStack _CleanupStack;

	const std::vector<const char*> _DeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
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

	void _InitializeAllocators()
	{
		TRACESTART;
		_FrameDataAllocator = new HostLocalAllocator(_Context, 8192*1024, false);
		_CleanupStack.push([=]() {
			LOGDBG("cleaning up allocators");
			_FrameDataAllocator->Cleanup();
			});
		TRACEEND;
	}

	void _InitializeDescriptorSet()
	{
		TRACESTART;

		initializeDescriptorPool(
			_Context,
			{
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10 }
			},
			&_FrameDataDescriptorPool
		);

		VkDescriptorSetLayoutBinding layoutBinding = getDescriptorSetLayoutBinding(
			0,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			1,
			VK_SHADER_STAGE_VERTEX_BIT,
			nullptr
		);

		auto objectLayoutBinding = getDescriptorSetLayoutBinding(
			0,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			1,
			VK_SHADER_STAGE_VERTEX_BIT,
			nullptr
		);

		_CleanupStack.push([=]() {
			LOGDBG("cleaning up frame data descriptor pool");
			cleanupDescriptorPool(_Context, _FrameDataDescriptorPool);
			});

		initializeDescriptorSetLayout(
			_Context.Device,
			1,
			&layoutBinding,
			&_FrameDataDescriptorSetLayout
		);

		initializeDescriptorSetLayout(
			_Context.Device,
			1,
			&objectLayoutBinding,
			&_ObjectDescriptorSetLayout
		);

		_CleanupStack.push([=]() {
			LOGDBG("cleaning up descriptor set layouts");
			cleanupDescriptorSetLayout(_Context.Device, _FrameDataDescriptorSetLayout);
			cleanupDescriptorSetLayout(_Context.Device, _ObjectDescriptorSetLayout);
		});

		for (size_t i = 0; i < FRAME_OVERLAP; i++)
		{
			_FrameData[i].Global.Data = {};
			_FrameData[i].Global.MemoryReference = _FrameDataAllocator->Allocate(
				&(_FrameData[i].Global.Data),
				sizeof(GlobalData),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
			);
			_FrameData[i].Global.MemoryReference.Transfer();

			_FrameData[i].Objects.MemoryReference = _FrameDataAllocator->Allocate(
				_FrameData[i].Objects.Data.data(),
				sizeof(GPUInstanceData) * _FrameData[i].Objects.Data.size(),
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			);
			_FrameData[i].Objects.MemoryReference.Transfer();

			initializeDescriptorSet(
				_Context,
				_FrameDataDescriptorPool,
				1,
				&_FrameDataDescriptorSetLayout,
				&(_FrameData[i].Global.DescriptorSet)
			);

			initializeDescriptorSet(
				_Context,
				_FrameDataDescriptorPool,
				1,
				&_ObjectDescriptorSetLayout,
				&(_FrameData[i].Objects.DescriptorSet)
			);

			updateDescriptorSet(
				_Context,
				_FrameData[i].Global.MemoryReference.Buffer,
				sizeof(GlobalData),
				0,
				_FrameData[i].Global.DescriptorSet,
				1,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			);

			updateDescriptorSet(
				_Context,
				_FrameData[i].Objects.MemoryReference.Buffer,
				sizeof(GPUInstanceData) * _FrameData[i].Objects.Data.size(),
				0,
				_FrameData[i].Objects.DescriptorSet,
				1,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			);
		}

		TRACEEND;
	}

	void _InitializePipeline()
	{
		TRACESTART;
		std::vector<VkDescriptorSetLayout> layouts = {
			_FrameDataDescriptorSetLayout, _ObjectDescriptorSetLayout
		};
		initializeGraphicsPipelineLayout(
			_Context.Device,
			layouts.size(),
			layouts.data(),
			0,
			nullptr,
			&_PipelineLayout
		);
		initializeStdPipeline(
			_Context.Device,
			"resources/shaders/TriangleVertex.vert.spv",
			"resources/shaders/TriangleVertex.frag.spv",
			_Swapchain.GetExtent(),
			VK_SAMPLE_COUNT_2_BIT,
			VK_COMPARE_OP_LESS,
			VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE,
			false,
			_PipelineLayout,
			_RenderPass,
			&_Pipeline
		);
		_CleanupStack.push([=]() {
			LOGDBG("cleaning up pipeline");
			cleanupGraphicsPipeline(_Context.Device, _Pipeline);
			cleanupGraphicsPipelineLayout(_Context.Device, _PipelineLayout);
			});
		TRACEEND;
	}

	void _InitializeSyncStructures()
	{
		TRACESTART;
		_RenderFence = new Fence(_Context, true);
		_PresentSemaphore = new Semaphore(_Context);
		_RenderSemaphore = new Semaphore(_Context);
		_CleanupStack.push([=]() {
			LOGDBG("cleaning up sync structures");
			_RenderFence->Cleanup();
			_PresentSemaphore->Cleanup();
			_RenderSemaphore->Cleanup();
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

	void _InitializeCommandBuffers()
	{
		TRACESTART;
		_MainCommandBuffer = new CommandBuffer(_Context, _CommandPool);
		_CleanupStack.push([=]() {
			LOGDBG("cleaning up command buffers");
			_MainCommandBuffer->Cleanup();
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

	void _InitializeModels()
	{
		RenderContext::GetInstance().Initialize(_Context);
		for (size_t i = 0; i < FRAME_OVERLAP; i++)
		{
			InstanceData d = {};
			d.ModelId = Models::SUZANNE;
			glm::vec3 pos = { 0,0,0 };
			d.GPUData.ModelMatrix = glm::identity<glm::mat4>();
			_FrameData[i].Objects.Data.push_back(d);
		}
	}
public:

	void Initialize(const char* title, SurfaceFactory* factory, WindowSize windowSize)
	{
		TRACESTART;
		_WindowSize = windowSize;
		_InitializeInstance(title);
#ifdef ENABLE_VALIDATION_LAYERS
		_InitializeDebugMessenger();
#endif
		_InitializeSurface(factory);
		_InitializePhysicalDevice();
		_InitializeLogicalDevice();

		_InitializeSwapchain(windowSize);
		_InitializeRenderPass();

		_Context.PhysicalDevice = _PhysicalDevice;
		_Context.Device = _Device;
		_Context.GraphicsQueue = _GraphicsQueue;
		_Context.PresentationQueue = _PresentationQueue;

		_InitializeSyncStructures();
		_InitializeModels();
		_InitializeAllocators();
		_InitializeDescriptorSet();
		_InitializePipeline();
		_InitializeCommandPool();
		_InitializeCommandBuffers();
		_InitializeRenderTargets();
		_InitializeFrameBuffer();
		TRACEEND;
	}

	void Render(float delta)
	{
		// TODO refactor
		_RenderFence->Wait();
		_RenderFence->Reset();
		uint32_t imageIndex;
		CheckVkResult(
			vkAcquireNextImageKHR(
				_Context.Device, _Swapchain.GetSwapchain(),
				1000000000, _PresentSemaphore->Instance, nullptr, &imageIndex
			)
		);
		_MainCommandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		std::array<VkClearValue, 2> clearValues;
		clearValues[0].color = { {.05f, .0f, .05f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };
		VkRenderPassBeginInfo rpInfo = {};
		rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		rpInfo.pNext = nullptr;
		rpInfo.renderPass = _RenderPass;
		rpInfo.renderArea.offset.x = 0;
		rpInfo.renderArea.offset.y = 0;
		rpInfo.renderArea.extent = _Swapchain.GetExtent();
		rpInfo.framebuffer = _SwapChainFramebuffers[imageIndex];
		rpInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		rpInfo.pClearValues = clearValues.data();
		vkCmdBeginRenderPass(
			_MainCommandBuffer->Buffer,
			&rpInfo,
			VK_SUBPASS_CONTENTS_INLINE
		);

		vkCmdBindPipeline(
			_MainCommandBuffer->Buffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			_Pipeline
		);

		// TODO frame overlap?
		TEST_CAMERA(_WindowSize.Width, _WindowSize.Height, delta, _MainCommandBuffer->Buffer, _PipelineLayout, &(_FrameData[0]));
		TEST_RENDER(_MainCommandBuffer->Buffer, _PipelineLayout, &(_FrameData[0]));

		vkCmdEndRenderPass(_MainCommandBuffer->Buffer);
		_MainCommandBuffer->End();
		_MainCommandBuffer->Submit(
			_GraphicsQueue,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			1,
			&(_PresentSemaphore->Instance),
			1,
			&(_RenderSemaphore->Instance),
			_RenderFence->Instance
		);

		auto swp = _Swapchain.GetSwapchain();
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.pSwapchains = &swp;
		presentInfo.swapchainCount = 1;
		presentInfo.pWaitSemaphores = &(_RenderSemaphore->Instance);
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pImageIndices = &imageIndex;
		CheckVkResult(vkQueuePresentKHR(_GraphicsQueue, &presentInfo));
	}

	void Cleanup()
	{
		TRACESTART;
		RenderContext::GetInstance().Cleanup(_Context);
		_CleanupStack.flush();
		TRACEEND;
	}
};