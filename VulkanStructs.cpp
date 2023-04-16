#include "VulkanStructs.h"


const VkCommandPoolCreateInfo VulkanStructs::CommandPoolCreateInfo(const VkCommandPoolResetFlags flags /*= 0*/)
{
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    return info;
}

const VkCommandBufferAllocateInfo VulkanStructs::CommandBufferAllocateInfo(const VkCommandPool pool, const uint32_t count /*= 1*/, const VkCommandBufferLevel level /*= VK_COMMAND_BUFFER_LEVEL_PRIMARY*/)
{
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.commandPool = pool;
    info.commandBufferCount = count;
    info.level = level;
    return info;
}

const VkCommandBufferBeginInfo VulkanStructs::CommandBufferBeginInfo(const VkCommandBufferUsageFlags flags /*= 0*/)
{
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.pNext = nullptr;
    info.pInheritanceInfo = nullptr;
    info.flags = flags;
    return info;
}

const VkFramebufferCreateInfo VulkanStructs::FramebufferCreateInfo(const VkRenderPass renderPass, const VkExtent2D extent)
{
    VkFramebufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.pNext = nullptr;
    info.renderPass = renderPass;
    info.attachmentCount = 1;
    info.width = extent.width;
    info.height = extent.height;
    info.layers = 1;
    return info;
}

const VkFenceCreateInfo VulkanStructs::FenceCreateInfo(const VkFenceCreateFlags flags /*= 0*/)
{
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    return info;
}

const VkSemaphoreCreateInfo VulkanStructs::SemaphoreCreateInfo(const VkSemaphoreCreateFlags flags /*= 0*/)
{
    VkSemaphoreCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    return info;
}

const VkSubmitInfo VulkanStructs::SubmitInfo(const VkCommandBuffer* cmd)
{
    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.pNext = nullptr;
    info.waitSemaphoreCount = 0;
    info.pWaitSemaphores = nullptr;
    info.pWaitDstStageMask = nullptr;
    info.commandBufferCount = 1;
    info.pCommandBuffers = cmd;
    info.signalSemaphoreCount = 0;
    info.pSignalSemaphores = nullptr;
    return info;
}

const VkPresentInfoKHR VulkanStructs::PresentInfo()
{
    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.pNext = nullptr;
    info.swapchainCount = 0;
    info.pSwapchains = nullptr;
    info.pWaitSemaphores = nullptr;
    info.waitSemaphoreCount = 0;
    info.pImageIndices = nullptr;
    return info;
}

const VkRenderPassBeginInfo VulkanStructs::RenderpassBeginInfo(const VkRenderPass renderPass, const VkExtent2D windowExtent, const VkFramebuffer framebuffer)
{
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.pNext = nullptr;
    info.renderPass = renderPass;
    info.renderArea.offset.x = 0;
    info.renderArea.offset.y = 0;
    info.renderArea.extent = windowExtent;
    info.clearValueCount = 0;
    info.pClearValues = nullptr;
    info.framebuffer = framebuffer;
    return info;
}

const VkPipelineShaderStageCreateInfo VulkanStructs::PipelineShaderStageCreateInfo(const VkShaderStageFlagBits stage, const VkShaderModule shaderModule)
{
    VkPipelineShaderStageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.stage = stage;
    info.module = shaderModule;
    info.pName = "main";
    return info;
}
const VkPipelineVertexInputStateCreateInfo VulkanStructs::VertexInputStateCreateInfo() {
    VkPipelineVertexInputStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.vertexBindingDescriptionCount = 0;
    info.vertexAttributeDescriptionCount = 0;
    return info;
}

const VkPipelineInputAssemblyStateCreateInfo VulkanStructs::InputAssemblyCreateInfo(const VkPrimitiveTopology topology) {
    VkPipelineInputAssemblyStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.topology = topology;
    info.primitiveRestartEnable = VK_FALSE;
    return info;
}
const VkPipelineRasterizationStateCreateInfo VulkanStructs::RasterizationStateCreateInfo(const VkPolygonMode polygonMode)
{
    VkPipelineRasterizationStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.depthClampEnable = VK_FALSE;
    info.rasterizerDiscardEnable = VK_FALSE;
    info.polygonMode = polygonMode;
    info.lineWidth = 1.0f;
    info.cullMode = VK_CULL_MODE_BACK_BIT;
    info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    info.depthBiasEnable = VK_FALSE;
    info.depthBiasConstantFactor = 0.0f;
    info.depthBiasClamp = 0.0f;
    info.depthBiasSlopeFactor = 0.0f;
    return info;
}
const VkPipelineMultisampleStateCreateInfo VulkanStructs::MultisamplingStateCreateInfo()
{
    VkPipelineMultisampleStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.sampleShadingEnable = VK_FALSE;
    info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    info.minSampleShading = 1.0f;
    info.pSampleMask = nullptr;
    info.alphaToCoverageEnable = VK_FALSE;
    info.alphaToOneEnable = VK_FALSE;
    return info;
}
const VkPipelineColorBlendAttachmentState VulkanStructs::ColorBlendAttachmentState() {
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = 
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    return colorBlendAttachment;
}
const VkPipelineLayoutCreateInfo VulkanStructs::PipelineLayoutCreateInfo() {
    VkPipelineLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.setLayoutCount = 0;
    info.pSetLayouts = nullptr;
    info.pushConstantRangeCount = 0;
    info.pPushConstantRanges = nullptr;
    return info;
}

const VkImageCreateInfo VulkanStructs::ImageCreateInfo(
    const VkFormat format,
    const VkImageUsageFlags usageFlags, 
    const VkExtent3D extent,
    const uint32_t arrayLayers,
    const VkImageCreateFlags createFlags
)
{
    VkImageCreateInfo info = { };
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = format;
    info.extent = extent;
    info.mipLevels = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usageFlags;
    info.arrayLayers = arrayLayers;
    info.flags = createFlags;
    return info;
}

const VkImageViewCreateInfo VulkanStructs::ImageviewCreateInfo(const VkFormat format, const VkImage image, const VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.pNext = nullptr;
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.image = image;
    info.format = format;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;
    info.subresourceRange.aspectMask = aspectFlags;
    return info;
}

const VkPipelineDepthStencilStateCreateInfo VulkanStructs::DepthStencilCreateInfo(const bool bDepthTest, const bool bDepthWrite, const VkCompareOp compareOp)
{
    VkPipelineDepthStencilStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.depthTestEnable = bDepthTest ? VK_TRUE : VK_FALSE;
    info.depthWriteEnable = bDepthWrite ? VK_TRUE : VK_FALSE;
    info.depthCompareOp = bDepthTest ? compareOp : VK_COMPARE_OP_ALWAYS;
    info.depthBoundsTestEnable = VK_FALSE;
    info.minDepthBounds = 0.0f; // Optional
    info.maxDepthBounds = 1.0f; // Optional
    info.stencilTestEnable = VK_FALSE;
    return info;
}

const VkDescriptorSetLayoutBinding VulkanStructs::DescriptorsetLayoutBinding(const VkDescriptorType type, const VkShaderStageFlags stageFlags, const uint32_t binding)
{
    VkDescriptorSetLayoutBinding setbind = {};
    setbind.binding = binding;
    setbind.descriptorCount = 1;
    setbind.descriptorType = type;
    setbind.pImmutableSamplers = nullptr;
    setbind.stageFlags = stageFlags;
    return setbind;
}
const VkWriteDescriptorSet VulkanStructs::WriteDescriptorBuffer(const VkDescriptorType type, const VkDescriptorSet dstSet, const VkDescriptorBufferInfo* bufferInfo, const uint32_t binding)
{
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;
    write.dstBinding = binding;
    write.dstSet = dstSet;
    write.descriptorCount = 1;
    write.descriptorType = type;
    write.pBufferInfo = bufferInfo;
    return write;
}

const VkWriteDescriptorSet VulkanStructs::WriteDescriptorImage(const VkDescriptorType type, const VkDescriptorSet dstSet, const VkDescriptorImageInfo* imageInfo, const uint32_t binding)
{
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;
    write.dstBinding = binding;
    write.dstSet = dstSet;
    write.descriptorCount = 1;
    write.descriptorType = type;
    write.pImageInfo = imageInfo;
    return write;
}

const VkSamplerCreateInfo VulkanStructs::SamplerCreateInfo(const VkFilter filters, const VkSamplerAddressMode samplerAdressMode /*= VK_SAMPLER_ADDRESS_MODE_REPEAT*/)
{
    VkSamplerCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    info.pNext = nullptr;
    info.magFilter = filters;
    info.minFilter = filters;
    info.addressModeU = samplerAdressMode;
    info.addressModeV = samplerAdressMode;
    info.addressModeW = samplerAdressMode;
    return info;
}

const VkBufferMemoryBarrier VulkanStructs::BufferBarrier(const VkBuffer buffer, const uint32_t queue)
{
    VkBufferMemoryBarrier barrier{};
    barrier.buffer = buffer;
    barrier.size = VK_WHOLE_SIZE;
    barrier.srcQueueFamilyIndex = queue;
    barrier.dstQueueFamilyIndex = queue;
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier.pNext = nullptr;
    return barrier;
}

const VkImageMemoryBarrier VulkanStructs::ImageBarrier(const VkImage image, const VkAccessFlags srcAccessMask, const VkAccessFlags dstAccessMask, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkImageAspectFlags aspectMask)
{
    VkImageMemoryBarrier result = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    result.srcAccessMask = srcAccessMask;
    result.dstAccessMask = dstAccessMask;
    result.oldLayout = oldLayout;
    result.newLayout = newLayout;
    result.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    result.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    result.image = image;
    result.subresourceRange.aspectMask = aspectMask;
    result.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    result.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
    return result;
}


const VkVertexInputBindingDescription VulkanStructs::VertexInputBindingDescription(
    const uint32_t binding,
    const uint32_t size,
    const VkVertexInputRate rate
)
{
    VkVertexInputBindingDescription result = {};
    result.binding = binding;
    result.stride = size;
    result.inputRate = rate;
    return result;
}

const std::vector<VkVertexInputAttributeDescription> VulkanStructs::VertexInputAttributeDescription(
    const uint32_t binding
)
{
    std::vector<VkVertexInputAttributeDescription> result = {};
    VkVertexInputAttributeDescription tmp = {};
    tmp.binding = binding;
    tmp.location = 0;
    tmp.format = VK_FORMAT_R32G32B32_SFLOAT;
    tmp.offset = offsetof(VertexData, Position);
    result.push_back(tmp);

    tmp = {};
    tmp.binding = binding;
    tmp.location = 1;
    tmp.format = VK_FORMAT_R32G32B32_SFLOAT;
    tmp.offset = offsetof(VertexData, Normal);
    result.push_back(tmp);

    tmp = {};
    tmp.binding = binding;
    tmp.location = 2;
    tmp.format = VK_FORMAT_R32G32_SFLOAT;
    tmp.offset = offsetof(VertexData, UV);
    result.push_back(tmp);

    tmp = {};
    tmp.binding = binding;
    tmp.location = 3;
    tmp.format = VK_FORMAT_R32G32B32_SFLOAT;
    tmp.offset = offsetof(VertexData, Tangent);
    result.push_back(tmp);

    tmp = {};
    tmp.binding = binding;
    tmp.location = 4;
    tmp.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    tmp.offset = offsetof(VertexData, Color);
    result.push_back(tmp);

    tmp = {};
    tmp.binding = binding;
    tmp.location = 5;
    tmp.format = VK_FORMAT_R32_UINT;
    tmp.offset = offsetof(VertexData, WeightsOffset);
    result.push_back(tmp);

    tmp = {};
    tmp.binding = binding;
    tmp.location = 6;
    tmp.format = VK_FORMAT_R32_UINT;
    tmp.offset = offsetof(VertexData, WeightsCount);
    result.push_back(tmp);

    tmp = {};
    tmp.binding = binding;
    tmp.location = 7;
    tmp.format = VK_FORMAT_R32_UINT;
    tmp.offset = offsetof(VertexData, BonesOffset);
    result.push_back(tmp);

    tmp = {};
    tmp.binding = binding;
    tmp.location = 8;
    tmp.format = VK_FORMAT_R32_UINT;
    tmp.offset = offsetof(VertexData, BonesCount);
    result.push_back(tmp);

    tmp = {};
    tmp.binding = binding;
    tmp.location = 9;
    tmp.format = VK_FORMAT_R32_SFLOAT;
    tmp.offset = offsetof(VertexData, Metallic);
    result.push_back(tmp);

    tmp = {};
    tmp.binding = binding;
    tmp.location = 10;
    tmp.format = VK_FORMAT_R32_SFLOAT;
    tmp.offset = offsetof(VertexData, Roughness);
    result.push_back(tmp);

    return result;
}