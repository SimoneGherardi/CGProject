#pragma once
#include "rendering_engine.h"
#include "camera.h"

void RenderingEngine::_InitializeInstance(const char* title)
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
	initializeInstance(&createInfo, &_Context.Instance);
	_CleanupStack.push([=]() {
		LOGDBG("cleaning up instance");
		cleanupInstance(_Context.Instance);
		});
	TRACEEND;
}

void RenderingEngine::_InitializeDebugMessenger()
{
	TRACESTART;
	auto createInfo = getDebugMessengerCreateInfo(
		DEBUG_CALLBACK,
		DEBUG_MESSENGER_SEVERITY_MASK(1, 1, 1, 1),
		DEBUG_MESSENGER_TYPE_MASK(1, 1, 1),
		nullptr
	);
	initializeDebugMessenger(_Context.Instance, &createInfo, nullptr, &_DebugMessenger);
	_CleanupStack.push([=]() {
		LOGDBG("cleaning up debug messenger");
		cleanupDebugMessenger(_Context.Instance, _DebugMessenger, nullptr);
		});
	TRACEEND;
}

void RenderingEngine::_InitializeSurface(SurfaceFactory* factory)
{
	TRACESTART;
	initializeSurface(_Context.Instance, &_Context.Surface, factory);
	_CleanupStack.push([=]() {
		LOGDBG("cleaning up surface");
		cleanupSurface(_Context.Instance, _Context.Surface);
		});
	TRACEEND;
}

void RenderingEngine::_InitializePhysicalDevice() {
	TRACESTART;
	auto devices = getPhysicalDevices(_Context.Instance);
	_Context.PhysicalDevice = getOptimalPhysicalDevice(devices, _Context.Surface, _DeviceExtensions);
	TRACEEND;
}

void RenderingEngine::_InitializeLogicalDevice() {
	TRACESTART;
	initializeLogicalDevice(
		_Context.PhysicalDevice,
		_Context.Surface,
		_ValidationLayers,
		_DeviceExtensions,
		&_Context.Device,
		&_Context.GraphicsQueue,
		&_Context.PresentationQueue
	);
	_CleanupStack.push([=]() {
		LOGDBG("cleaning up logical device");
		cleanupLogicalDevice(_Context.Device);
		});
	TRACEEND;
}

void RenderingEngine::_InitializeSwapchain(WindowSize windowSize) {
	TRACESTART;
	_Swapchain.Initialize(
		windowSize.Width, windowSize.Height,
		_Context.PhysicalDevice, _Context.Device, _Context.Surface
	);
	_CleanupStack.push([=]() {
		LOGDBG("cleaning up logical device");
		_Swapchain.Cleanup();
		});
	TRACEEND;
}

void RenderingEngine::_InitializeRenderPass() {
	TRACESTART;
	initializeRenderPass(
		_Context.PhysicalDevice,
		_Context.Device,
		_Swapchain.GetFormat(),
		VK_SAMPLE_COUNT_2_BIT,
		&_RenderPass
	);
	_CleanupStack.push([=]() {
		LOGDBG("cleaning up render pass");
		cleanupRenderPass(_Context.Device, _RenderPass);
		});
	TRACEEND;
}

void RenderingEngine::_InitializeGUIRenderPass() {
	TRACESTART;
	initializeRenderPass(
		_Context.PhysicalDevice,
		_Context.Device,
		_Swapchain.GetFormat(),
		VK_SAMPLE_COUNT_2_BIT,
		&_GUIRenderPass
	);
	_CleanupStack.push([=]() {
		LOGDBG("cleaning up render pass");
		cleanupRenderPass(_Context.Device, _GUIRenderPass);
		});
	TRACEEND;
}

void RenderingEngine::_InitializeAllocators()
{
	TRACESTART;
	// _FrameDataAllocator = new HostLocalAllocator(_Context, 8192*1024, false);
	_HostVisibleMemory = new DeviceMemory(&_Context, HOST_VISIBLE_MEMORY_SIZE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	_DeviceLocalMemory = new DeviceMemory(&_Context, DEVICE_LOCAL_MEMORY_SIZE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	_CleanupStack.push([=]() {
		LOGDBG("cleaning up allocators");
		// _FrameDataAllocator->Cleanup();
		_HostVisibleMemory->Cleanup();
		_DeviceLocalMemory->Cleanup();
		});
	TRACEEND;
}

void RenderingEngine::_InitializeDescriptorSet()
{
	TRACESTART;

	initializeDescriptorPool(
		&_Context,
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
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		nullptr
	);

	auto objectLayoutBinding = getDescriptorSetLayoutBinding(
		0,
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		1,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		nullptr
	);

	_CleanupStack.push([=]() {
		LOGDBG("cleaning up frame data descriptor pool");
		cleanupDescriptorPool(&_Context, _FrameDataDescriptorPool);
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
		auto f = GetCurrentFrameData();
		f->Global.Data = {};
		f->Global.Buffer = _HostVisibleMemory->NewBuffer(
			sizeof(GlobalData),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
		);
		f->Global.Update(&_Context);

		f->Objects.Buffer = _HostVisibleMemory->NewBuffer(
			sizeof(GPUInstanceData) * MAX_INSTANCES,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
		);
		f->Objects.CommandsBuffer = _HostVisibleMemory->NewBuffer(
			sizeof(VkDrawIndexedIndirectCommand) * MAX_COMMANDS,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
		);

		f->Objects.Update(&_Context);

		initializeDescriptorSet(
			&_Context,
			_FrameDataDescriptorPool,
			1,
			&_FrameDataDescriptorSetLayout,
			&(f->Global.DescriptorSet)
		);

		initializeDescriptorSet(
			&_Context,
			_FrameDataDescriptorPool,
			1,
			&_ObjectDescriptorSetLayout,
			&(f->Objects.DescriptorSet)
		);

		updateDescriptorSet(
			&_Context,
			f->Global.Buffer.Buffer,
			sizeof(GlobalData),
			0,
			f->Global.DescriptorSet,
			1,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
		);
	}
	TRACEEND;
}

void RenderingEngine::_InitializePipeline()
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

void RenderingEngine::_InitializeSyncStructures()
{
	TRACESTART;
	_RenderFence = new Fence(&_Context, true);
	_PresentSemaphore = new Semaphore(&_Context);
	_RenderSemaphore = new Semaphore(&_Context);
	_CleanupStack.push([=]() {
		LOGDBG("cleaning up sync structures");
		_RenderFence->Cleanup();
		_PresentSemaphore->Cleanup();
		_RenderSemaphore->Cleanup();
		});
	TRACEEND;
}

void RenderingEngine::_InitializeCommandPool() {
	TRACESTART;
	auto queueFamilyIndices = findQueueFamilies(_Context.PhysicalDevice, _Context.Surface);
	initializeCommandPool(
		_Context.Device,
		queueFamilyIndices,
		&_CommandPool
	);
	_CleanupStack.push([=]() {
		LOGDBG("cleaning up command pool");
		cleanupCommandPool(_Context.Device, _CommandPool);
		});
	TRACEEND;
}

void RenderingEngine::_InitializeGUICommandPool() {
	TRACESTART;
	auto queueFamilyIndices = findQueueFamilies(_Context.PhysicalDevice, _Context.Surface);
	initializeCommandPool(
		_Context.Device,
		queueFamilyIndices,
		&_GUICommandPool
	);
	_CleanupStack.push([=]() {
		LOGDBG("cleaning up command pool");
		cleanupCommandPool(_Context.Device, _GUICommandPool);
		});
	TRACEEND;
}

void RenderingEngine::_InitializeCommandBuffers()
{
	TRACESTART;
	_MainCommandBuffer = new CommandBuffer(&_Context, _CommandPool);
	_CleanupStack.push([=]() {
		LOGDBG("cleaning up command buffers");
		_MainCommandBuffer->Cleanup();
		});
	TRACEEND;
}

void RenderingEngine::_InitializeGUICommandBuffers()
{
	TRACESTART;
	_GUICommandBuffer = new CommandBuffer(&_Context, _CommandPool);
	_CleanupStack.push([=]() {
		LOGDBG("cleaning up command buffers");
		_GUICommandBuffer->Cleanup();
		});
	TRACEEND;
}

void RenderingEngine::_InitializeRenderTargets()
{
	TRACESTART;
	_ColorRenderTarget.Initialize(
		_Context.PhysicalDevice,
		_Context.Device,
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
		_Context.PhysicalDevice,
		_Context.Device,
		_Swapchain.GetExtent(),
		findDepthFormat(_Context.PhysicalDevice),
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

void RenderingEngine::_InitializeFrameBuffer() {
	TRACESTART;
	_SwapChainFramebuffers.resize(_Swapchain.GetImagesViews().size());
	for (size_t i = 0; i < _Swapchain.GetImagesViews().size(); i++) {
		std::array<VkImageView, 3> attachments = {
			_ColorRenderTarget.GetImageView(),
			_DepthRenderTarget.GetImageView(),
			_Swapchain.GetImagesViews()[i]
		};

		initializeFrameBuffer(
			_Context.Device,
			static_cast<uint32_t>(attachments.size()),
			attachments.data(),
			_RenderPass,
			_Swapchain.GetExtent(),
			&_SwapChainFramebuffers[i]
		);

		_CleanupStack.push([=]() {
			LOGDBG("cleaning up framebuffer");
			cleanupFrameBuffer(
				_Context.Device,
				_SwapChainFramebuffers[i]
			);
			});
	}
	TRACEEND;
}

void RenderingEngine::_InitializeGUIFrameBuffer() {
	TRACESTART;
	_GUISwapChainFramebuffers.resize(_Swapchain.GetImagesViews().size());
	for (size_t i = 0; i < _Swapchain.GetImagesViews().size(); i++) {
		std::array<VkImageView, 3> attachments = {
			_ColorRenderTarget.GetImageView(),
			_DepthRenderTarget.GetImageView(),
			_Swapchain.GetImagesViews()[i]
		};

		initializeFrameBuffer(
			_Context.Device,
			static_cast<uint32_t>(attachments.size()),
			attachments.data(),
			_RenderPass,
			_Swapchain.GetExtent(),
			&_GUISwapChainFramebuffers[i]
		);

		_CleanupStack.push([=]() {
			LOGDBG("cleaning up framebuffer");
			cleanupFrameBuffer(
				_Context.Device,
				_GUISwapChainFramebuffers[i]
			);
			});
	}
	TRACEEND;
}

void RenderingEngine::_InitializeModels()
{
	TRACESTART;
	Buffer stagingBuffer = _HostVisibleMemory->NewBuffer(1024 * 1024, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	RenderContext::GetInstance().Initialize(&_Context, stagingBuffer, _DeviceLocalMemory);
	_HostVisibleMemory->FreeBuffer(stagingBuffer);
	TRACEEND;
}

void RenderingEngine::_InitializeGui()
{
	TRACESTART;

	

	initializeDescriptorPool(
		&_Context,
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		},
		&_GuiDescriptorPool
	);

	/*
	std::array<VkCommandBuffer, 3> submitCommandBuffers = {
	environmentalcommandBuffers[imageIndex].handle,
	commandBuffers[imageIndex].handle,
	imGuiCommandBuffers[imageIndex].handle
				};
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = static_cast<uint32_t>(submitCommandBuffers.size());
	submitInfo.pCommandBuffers = submitCommandBuffers.data();
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsQueue_, 1, &submitInfo, synchronization.inFlightFences[imageIndex].handle) != VK_SUCCESS)
	throw std::runtime_error("failed to submit draw command buffer!");
	*/

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = _Context.Instance;
	init_info.PhysicalDevice = _Context.PhysicalDevice;
	init_info.Device = _Context.Device;
	init_info.Queue = _Context.GraphicsQueue;
	init_info.DescriptorPool = _GuiDescriptorPool;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;
	init_info.MSAASamples = VK_SAMPLE_COUNT_2_BIT;

	ImGui_ImplVulkan_Init(&init_info, _RenderPass);

	_GuiCommandBuffer = new ImmediateCommandBuffer(&_Context);

	_GuiCommandBuffer->Submit([&](VkCommandBuffer cmd) {
		ImGui_ImplVulkan_CreateFontsTexture(cmd);
		});
	_GuiCommandBuffer->Wait();

	ImGui_ImplVulkan_DestroyFontUploadObjects();

	_CleanupStack.push([=]() {
		LOGDBG("cleaning up gui");
		_GuiCommandBuffer->Cleanup();
		cleanupDescriptorPool(&_Context, _GuiDescriptorPool);
		ImGui_ImplVulkan_Shutdown();
		});
	TRACEEND;
}

void RenderingEngine::Initialize(const char* title, SurfaceFactory* factory, WindowSize windowSize)
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
	_InitializeGUIRenderPass();

	_InitializeSyncStructures();
	_InitializeAllocators();
	_InitializeModels();
	_InitializeDescriptorSet();
	_InitializePipeline();
	_InitializeCommandPool();
	_InitializeGUICommandPool();
	_InitializeCommandBuffers();
	_InitializeGUICommandBuffers();
	_InitializeRenderTargets();
	_InitializeFrameBuffer();
	_InitializeGUIFrameBuffer();
	_InitializeGui();
	TRACEEND;
}

void RenderingEngine::Render(float delta, CameraTest* camera)
{
	// TODO refactor
	_RenderFence->Wait();
	_RenderFence->Reset();
	ImGui::Render();
	uint32_t imageIndex;
	CheckVkResult(
		vkAcquireNextImageKHR(
			_Context.Device, _Swapchain.GetSwapchain(),
			1000000000, _PresentSemaphore->Instance, nullptr, &imageIndex
		)
	);
	_MainCommandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	VkCommandBuffer cmd = _MainCommandBuffer->Buffer;

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
		cmd,
		&rpInfo,
		VK_SUBPASS_CONTENTS_INLINE
	);

	vkCmdBindPipeline(
		cmd,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		_Pipeline
	);

	// TODO frame overlap?
	auto f = GetCurrentFrameData();
	f->Global.Data.CameraViewProjection = camera->Matrix(70, 0.1f, 200);
	// TODO update projection
	// TODO update view

	TEST_CAMERA(&_Context, _WindowSize.Width, _WindowSize.Height, delta, cmd, _PipelineLayout, f);

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(cmd, 0, 1, &(RenderContext::GetInstance().VertexBuffer.Buffer), &offset);
	vkCmdBindIndexBuffer(cmd, RenderContext::GetInstance().IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdDrawIndexedIndirect(cmd, f->Objects.CommandsBuffer.Buffer, 0, f->Objects.Commands.size(), sizeof(VkDrawIndexedIndirectCommand));

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

	vkCmdEndRenderPass(cmd);
	_MainCommandBuffer->End();
	_MainCommandBuffer->Submit(
		_Context.GraphicsQueue,
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
	CheckVkResult(vkQueuePresentKHR(_Context.GraphicsQueue, &presentInfo));
}

void RenderingEngine::Cleanup()
{
	TRACESTART;
	RenderContext::GetInstance().Cleanup(&_Context);
	_CleanupStack.flush();
	TRACEEND;
}

FrameData* RenderingEngine::GetCurrentFrameData()
{
	return &(_FrameData[_CurrentFrameIndex]);
}

RenderingEngine& RenderingEngine::GetInstance()
{
	static RenderingEngine instance;
	return instance;
}