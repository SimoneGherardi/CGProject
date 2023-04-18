#include "ImageCubeMap.h"
#include <stb_image.h>

void __rotatePixels(
	const uint32_t sideLength,
	const uint32_t offX,
	const uint32_t offY,
    uint32_t* pixels
)
{
	const uint32_t s = sideLength;
	const uint32_t xEnd = s - offX - 1;
	const uint32_t yEnd = s - offY - 1;
	uint32_t* pLT = pixels + offX + s * offY;
	uint32_t* pRT = pixels + yEnd + s * offX;
	uint32_t* pRB = pixels + xEnd + s * yEnd;
	uint32_t* pLB = pixels + offY + s * xEnd;
	const uint32_t tmp = *pLT;
	*pLT = *pLB;
	*pLB = *pRB;
	*pRB = *pRT;
	*pRT = tmp;
}

void __applyRotation(
	const uint32_t width,
    const uint32_t height,
	const SkyboxFaceRotation rotation,
    uint8_t* pixels
)
{
	assert(width == height);
	for (uint32_t steps = 0; steps < rotation; steps++)
	{
		for (uint32_t offX = 0; offX < width / 2; offX++)
		{
			for (uint32_t offY = 0; offY < height / 2; offY++)
			{
				__rotatePixels(width, offX, offY, (uint32_t*) pixels);
			}
		}
	}
}

// TODO make a memory transferer
void __copyPixelsToImage(
    const VulkanContext context,
    const uint32_t width,
    const uint32_t height,
    const uint32_t channels,
    const uint8_t* pixels,
    DeviceMemory* stagingMemory,
    VkImage image,
    const uint32_t baseLayer = 0,
	const uint32_t mipLevel = 0
)
{
	ImmediateCommandBuffer immediate = ImmediateCommandBuffer(context);
	auto size = width * height * channels;

	Buffer stagingBuffer = stagingMemory->NewBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	MemoryTransferer m = MemoryTransferer(context, stagingBuffer, pixels, size);
	m.TransferMapped();

	immediate.Submit([=](VkCommandBuffer cmd) {
		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = mipLevel;
		region.imageSubresource.baseArrayLayer = baseLayer;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };


		vkCmdCopyBufferToImage(
			cmd,
			stagingBuffer.Buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);
	});
	immediate.Wait();
	stagingMemory->FreeBuffer(stagingBuffer);
	immediate.Cleanup();
}


// TODO move in a library
void __transitionImageLayout(VulkanContext context, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t layers, uint32_t startMip, uint32_t mipLevels)
{
	ImmediateCommandBuffer immediate = ImmediateCommandBuffer(context);
	immediate.Submit([=](VkCommandBuffer cmd) {
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;

		//these are used to transfer queue ownership, which we aren't doing
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = startMip;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = layers;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			//Unsupported layout transition
			throw new std::runtime_error("Attempting an unsupported image layout transition");
		}

		vkCmdPipelineBarrier(
			cmd,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	});
	immediate.Wait();
	immediate.Cleanup();
}





SkyboxImageBuilder SkyboxImageBuilder::Prepare(
    const VulkanContext context,
	const uint32_t mipLevels,
    DeviceMemory* stagingMemory,
    DeviceMemory* destinationMemory
)
{
    assert(State == START);
    Context = context;
	MipLevels = mipLevels;
    StagingMemory = stagingMemory;
    DestinationMemory = destinationMemory;
    State = PREPARED;
    return *this;
}

SkyboxImageBuilder SkyboxImageBuilder::AddFace(
    const std::string filename,
    const SkyboxFace face,
	const SkyboxFaceRotation rotation
)
{
    int width, height, channels;
    assert(State == PREPARED || State == BUILDING);
    uint8_t* data = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	channels = 4;
    AddFacePixeldata(width, height, channels, data, face, rotation);
    State = BUILDING;
    return *this;
}

SkyboxImageBuilder SkyboxImageBuilder::AddFacePixeldata(
    const uint32_t width,
    const uint32_t height,
    const uint32_t channels,
    uint8_t* data,
    const SkyboxFace face,
	const SkyboxFaceRotation rotation
)
{
    assert(State == PREPARED || State == BUILDING);
    if (State == PREPARED) {
        Width = width;
        Height = height;
        Channels = channels;
        ImageBuffer = DestinationMemory->NewImage(
            Width,
            Height,
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            6,
            VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
			MipLevels
        );
        __transitionImageLayout(Context, ImageBuffer.Image, ImageBuffer.Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 6, 0, MipLevels);
    }
    assert(Width == width);
    assert(Height == height);
    assert(Channels == channels);
	__applyRotation(width, height, rotation, data);
    __copyPixelsToImage(Context, Width, Height, Channels, data, StagingMemory, ImageBuffer.Image, face, 0);
    State = BUILDING;
    return *this;
}

// SkyboxImageBuilder SkyboxImageBuilder::AddSkyboxTexture(
//     const std::string filename,
//     const uint32_t rows = 3, const uint32_t columns = 4,
//     const uint32_t frontIndex = 5, const uint32_t backIndex = 7,
//     const uint32_t upIndex = 1, const uint32_t downIndex = 9,
//     const uint32_t leftIndex = 4, const uint32_t rightIndex = 6
// )
// {
//     int width, height, channels;
//     assert(State == PREPARED || State == BUILDING);
//     uint8_t* data = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
//     assert(width % columns == 0);
//     assert(height % rows == 0);
//     uint32_t blockWidth = width / columns;
//     uint32_t blockHeight = height / rows;

// }

SkyboxImage SkyboxImageBuilder::Build()
{
    assert(State == BUILDING);
	// build mipmaps
	__transitionImageLayout(Context, ImageBuffer.Image, ImageBuffer.Format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 6, 0, 1);
	ImmediateCommandBuffer buf = ImmediateCommandBuffer(Context);
	int32_t size = Width;
	for (int i = 1; i < MipLevels; i++)
	{
		VkImageBlit blit = {};
		blit.srcOffsets[0] = { 0,0,0 };
		blit.srcOffsets[1] = { size, size, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 6;
		blit.dstOffsets[0] = { 0,0,0 };
		blit.dstOffsets[1] = { size > 1 ? size/2 : 1, size > 1 ? size/2 : 1, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 6;
		buf.Submit([=](VkCommandBuffer cmd) {
			vkCmdBlitImage(cmd,
				ImageBuffer.Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				ImageBuffer.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit, VK_FILTER_LINEAR
			);
		});
		buf.Wait();
		__transitionImageLayout(Context, ImageBuffer.Image, ImageBuffer.Format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 6, i, 1);
		if (size > 1) { size /= 2; }
	}
	buf.Cleanup();
	__transitionImageLayout(Context, ImageBuffer.Image, ImageBuffer.Format, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 6, 0, MipLevels);
	VkImageView view = initializeImageView(
		Context->Device,
		ImageBuffer.Image,
		ImageBuffer.Format,
		VK_IMAGE_ASPECT_COLOR_BIT,
		MipLevels,
		VK_IMAGE_VIEW_TYPE_CUBE,
		6
	);
    VkSampler sampler;
    VkSamplerCreateInfo samplerCreateInfo = VulkanStructs::SamplerCreateInfo(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	samplerCreateInfo.mipLodBias = 0;
	samplerCreateInfo.minLod = 0;
	samplerCreateInfo.maxLod = MipLevels;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	CheckVkResult(vkCreateSampler(Context->Device, &samplerCreateInfo, nullptr, &sampler));

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = view;
    imageInfo.sampler = sampler;

    SkyboxImage i = {};
    i.Width = Width;
    i.Height = Height;
    i.Channels = Channels;
    i.Image = ImageBuffer.Image;
    i.ImageView = view;
    i.Sampler = sampler;
    i.ImageInfo = imageInfo;
	i.Context = Context;
    State = BUILT;

    return i;
}

void SkyboxImage::Cleanup()
{
	vkDestroyImageView(Context->Device, ImageView, nullptr);
	vkDestroySampler(Context->Device, Sampler, nullptr);
}