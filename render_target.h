#pragma once
#include "defines.h"
#include "swap_chain_info.h"
#include "image.h"
#include "image_views.h"

class RenderTarget
{
private:
	VkImage _Image;
	VkDeviceMemory _Memory;
	VkImageView _ImageView;
	VkDevice _Device;
public:
	void Initialize(
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
	);
	void Cleanup();
	VkImage GetImage() const;
	VkDeviceMemory GetMemory() const;
	VkImageView GetImageView() const;
};