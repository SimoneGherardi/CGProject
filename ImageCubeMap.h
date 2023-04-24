#pragma once
#include "defines.h"
#include "asset_types.hpp"
#include "DeviceMemory.h"
#include "ImmediateCommandBuffer.h"
#include "MemoryTransferer.h"
#include "image_views.h"
#include "VulkanStructs.h"

struct SkyboxImage
{
    VulkanContext Context;

    VkImage Image;
    VkImageView ImageView;
    VkSampler Sampler;
    VkDescriptorImageInfo ImageInfo;

    uint32_t Width;
    uint32_t Height;
    uint32_t Channels;

    void Cleanup();
};

enum SkyboxFaceRotation
{
    NONE = 0,
    CW_90 = 1,
    CW_180 = 2,
    CW_270 = 3
};

enum SkyboxFace
{
    FRONT = 0,
    BACK = 1,
    UP = 2,
    DOWN = 3,
    RIGHT = 4,
    LEFT = 5
};

enum SkyboxImageBuilderState
{
    START,
    PREPARED,
    BUILDING,
    BUILT
};

struct SkyboxImageBuilder
{
    SkyboxImageBuilderState State = START;
    uint32_t Width;
    uint32_t Height;
    uint32_t Channels;
    uint32_t MipLevels;

    VulkanContext Context;
    DeviceMemory* StagingMemory;
    DeviceMemory* DestinationMemory;

    Image ImageBuffer;

    SkyboxImageBuilder Prepare(
        const VulkanContext context,
        const uint32_t mipLevels,
        DeviceMemory* stagingMemory,
        DeviceMemory* destinationMemory
    );

    SkyboxImageBuilder AddFace(
        const std::string filename,
        const SkyboxFace face,
        const SkyboxFaceRotation rotation = NONE
    );

    SkyboxImageBuilder AddFacePixeldata(
        const uint32_t width,
        const uint32_t height,
        const uint32_t channels,
        uint8_t* data,
        const SkyboxFace face,
        const SkyboxFaceRotation rotation = NONE
    );

    SkyboxImage Build();
};
