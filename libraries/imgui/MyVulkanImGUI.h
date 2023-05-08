#pragma once
#include "imgui.h"      
#if defined(IMGUI_IMPL_VULKAN_NO_PROTOTYPES) && !defined(VK_NO_PROTOTYPES)
#define VK_NO_PROTOTYPES
#endif
#include <vulkan/vulkan.h>

struct Vulkan_InitInfo_GUI
{
    VkInstance                      Instance;
    VkPhysicalDevice                PhysicalDevice;
    VkDevice                        Device;
    uint32_t                        QueueFamily;
    VkQueue                         Queue;
    VkPipelineCache                 PipelineCache;
    VkDescriptorPool                DescriptorPool;
    uint32_t                        Subpass;
    uint32_t                        MinImageCount;          
    uint32_t                        ImageCount;             
    VkSampleCountFlagBits           MSAASamples;            
    const VkAllocationCallbacks*    Allocator;
    void                            (*CheckVkResultFn)(VkResult err);
};

IMGUI_IMPL_API bool         Vulkan_Init_GUI(Vulkan_InitInfo_GUI* info, VkRenderPass render_pass);
IMGUI_IMPL_API void         Vulkan_Cleanup_GUI();
IMGUI_IMPL_API void         Vulkan_NewFrame_GUI();
IMGUI_IMPL_API void         Vulkan_RenderDrawData_GUI(ImDrawData* draw_data, VkCommandBuffer command_buffer, VkPipeline pipeline = VK_NULL_HANDLE);
IMGUI_IMPL_API bool         Vulkan_CreateFontsTexture_GUI(VkCommandBuffer command_buffer);
IMGUI_IMPL_API void         Vulkan_DestroyFontUploadObjects_GUI();
IMGUI_IMPL_API void         Vulkan_SetMinImageCount_GUI(uint32_t min_image_count); // To override MinImageCount after initialization (e.g. if swap chain is recreated)

IMGUI_IMPL_API VkDescriptorSet Vulkan_AddTexture_GUI(VkSampler sampler, VkImageView image_view, VkImageLayout image_layout);
IMGUI_IMPL_API void            Vulkan_RemoveTexture_GUI(VkDescriptorSet descriptor_set);

IMGUI_IMPL_API bool         Vulkan_LoadFunctions_GUI(PFN_vkVoidFunction(*loader_func)(const char* function_name, void* user_data), void* user_data = nullptr);

struct Vulkan_Frame;
struct Vulkan_Window;

IMGUI_IMPL_API void                 Vulkan_DestroyWindow_GUI(VkInstance instance, VkDevice device, Vulkan_Window* wnd, const VkAllocationCallbacks* allocator);

struct Vulkan_Frame
{
    VkCommandPool       CommandPool;
    VkCommandBuffer     CommandBuffer;
    VkFence             Fence;
    VkImage             Backbuffer;
    VkImageView         BackbufferView;
    VkFramebuffer       Framebuffer;
};

struct Vulkan_FrameSemaphores
{
    VkSemaphore         ImageAcquiredSemaphore;
    VkSemaphore         RenderCompleteSemaphore;
};

struct Vulkan_Window
{
    int                 Width;
    int                 Height;
    VkSwapchainKHR      Swapchain;
    VkSurfaceKHR        Surface;
    VkSurfaceFormatKHR  SurfaceFormat;
    VkPresentModeKHR    PresentMode;
    VkRenderPass        RenderPass;
    VkPipeline          Pipeline;               
    bool                ClearEnable;
    VkClearValue        ClearValue;
    uint32_t            FrameIndex;             
    uint32_t            ImageCount;             
    uint32_t            SemaphoreIndex;         
    Vulkan_Frame*            Frames;
    Vulkan_FrameSemaphores*  FrameSemaphores;

    Vulkan_Window()
    {
        memset((void*)this, 0, sizeof(*this));
        PresentMode = (VkPresentModeKHR)~0;     
        ClearEnable = true;
    }
};
