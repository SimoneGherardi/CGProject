#include "render_target.h"

void RenderTarget::Initialize(
	const VkPhysicalDevice physicalDevice,
	const VkDevice device,
	const VkExtent2D extent,
	const VkFormat format,
	const uint32_t mipMapLevels,
	const VkSampleCountFlagBits msaaSamples,
	const VkImageTiling imageTiling,
	const VkImageUsageFlags usageFlags,
	const VkMemoryPropertyFlags memPropFlags,
	const VkImageAspectFlags aspectFlags
)
{
	_Device = device;
	initializeImage(
		physicalDevice,
		device,
		extent.width,
		extent.height,
		mipMapLevels,
		msaaSamples,
		format,
		imageTiling,
		usageFlags,
		memPropFlags,
		&_Image,
		&_Memory
	);
	_ImageView = initializeImageView(
		device,
		_Image,
		format,
		aspectFlags,
		mipMapLevels,
		VK_IMAGE_VIEW_TYPE_2D,
		1
	);
}

void RenderTarget::Cleanup()
{
	cleanupImageView(_Device, _ImageView);
	cleanupImage(_Device, _Image, _Memory);
}

VkImage RenderTarget::GetImage() const
{
	return _Image;
}
VkDeviceMemory RenderTarget::GetMemory() const
{
	return _Memory;
}
VkImageView RenderTarget::GetImageView() const
{
	return _ImageView;
}