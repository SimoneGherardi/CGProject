#include "render_passes.h"

VkAttachmentDescription getAttachmentDescription(
    const VkFormat format,
    const VkSampleCountFlagBits samples,
    const VkAttachmentLoadOp loadOp,
    const VkAttachmentStoreOp storeOp,
    const VkAttachmentLoadOp stencilLoadOp,
    const VkAttachmentStoreOp stencilStoreOp,
    const VkImageLayout initialLayout,
    const VkImageLayout finalLayout
)
{
    VkAttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = format;
    colorAttachmentResolve.samples = samples;
    colorAttachmentResolve.loadOp = loadOp;
    colorAttachmentResolve.storeOp = storeOp;
    colorAttachmentResolve.stencilLoadOp = stencilLoadOp;
    colorAttachmentResolve.stencilStoreOp = stencilStoreOp;
    colorAttachmentResolve.initialLayout = initialLayout;
    colorAttachmentResolve.finalLayout = finalLayout;
    return colorAttachmentResolve;
}

VkAttachmentReference getAttachmentReference(
    const uint32_t attachment,
    const VkImageLayout layout
)
{
    VkAttachmentReference colorAttachmentResolveRef{};
    colorAttachmentResolveRef.attachment = attachment;
    colorAttachmentResolveRef.layout = layout;
    return colorAttachmentResolveRef;
}

VkSubpassDescription getSubpassDescription(
    const VkPipelineBindPoint pipelineBindPoint,
    const uint32_t colorAttachmentCount,
    const VkAttachmentReference* colorAttachmentRef,
    const VkAttachmentReference* depthAttachmentRef,
    const VkAttachmentReference* colorAttachmentResolveRef
)
{
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = colorAttachmentRef;
    subpass.pDepthStencilAttachment = depthAttachmentRef;
    subpass.pResolveAttachments = colorAttachmentResolveRef;
    return subpass;
}

VkSubpassDependency getSubpassDependency(
    const uint32_t srcSubpass,
    const uint32_t dstSubpass,
    const VkPipelineStageFlags srcStageMask,
    const VkAccessFlags srcAccessMask,
    const VkPipelineStageFlags dstStageMask,
    const VkAccessFlags dstAccessMask
)
{
    VkSubpassDependency dependency{};
    dependency.srcSubpass = srcSubpass;
    dependency.dstSubpass = dstSubpass;
    dependency.srcStageMask = srcStageMask;
    dependency.srcAccessMask = srcAccessMask;
    dependency.dstStageMask = dstStageMask;
    dependency.dstAccessMask = dstAccessMask;
    return dependency;
}

void initializeRenderPass(
    const VkPhysicalDevice physicalDevice,
    const VkDevice device,
    const VkFormat format,
    const VkSampleCountFlagBits msaaSamples,
    VkRenderPass* renderPass
) {
    auto colorAttachmentResolve = getAttachmentDescription(
        format,
        VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    );

    auto colorAttachmentResolveRef = getAttachmentReference(
        2,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    );

    auto depthAttachment = getAttachmentDescription(
        findDepthFormat(physicalDevice),
        msaaSamples,
        VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    );

    auto depthAttachmentRef = getAttachmentReference(
        1,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    );

    auto colorAttachment = getAttachmentDescription(
        format,
        msaaSamples,
        VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    );

    auto colorAttachmentRef = getAttachmentReference(
        0,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    );

    auto subpass = getSubpassDescription(
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        1,
        &colorAttachmentRef,
        &depthAttachmentRef,
        &colorAttachmentResolveRef
    );

    auto dependency = getSubpassDependency(
        VK_SUBPASS_EXTERNAL,
        0,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        0,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
    );

    std::vector<VkAttachmentDescription> attachments = {
        colorAttachment,
        depthAttachment,
        colorAttachmentResolve
    };

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkResult result = vkCreateRenderPass(
        device, 
        &renderPassInfo, 
        nullptr,
        renderPass
    );
    if (result != VK_SUCCESS) {
        PrintVkError(result);
        throw std::runtime_error("failed to create render pass!");
    }
}

void cleanupRenderPass(const VkDevice device, VkRenderPass renderPass)
{
    vkDestroyRenderPass(device, renderPass, nullptr);
}

VkFormat findDepthFormat(const VkPhysicalDevice physicalDevice) {
    return findSupportedFormat(
        physicalDevice,
        {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
        },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

VkFormat findSupportedFormat(
    const VkPhysicalDevice physicalDevice,
    const std::vector<VkFormat> candidates,
    const VkImageTiling tiling,
    const VkFormatFeatureFlags features
)
{
    for (VkFormat format : candidates) {
        VkFormatProperties props;

        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures & features) == features) {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
            (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}