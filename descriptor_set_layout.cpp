#include "descriptor_set_layout.h"

VkDescriptorSetLayoutBinding getDescriptorSetLayoutBinding(
	const uint32_t binding,
	const VkDescriptorType descriptorType,
	const uint32_t descriptorCount,
	const VkShaderStageFlags stageFlags,
	const VkSampler* immutableSamplers
)
{
	VkDescriptorSetLayoutBinding r{};
	r.binding = binding;
	r.descriptorType = descriptorType;
	r.descriptorCount = descriptorCount;
	r.stageFlags = stageFlags;
	r.pImmutableSamplers = immutableSamplers;
	return r;
}

void initializeDescriptorSetLayout(
	const VkDevice device,
	const uint32_t bindingsCount,
	const VkDescriptorSetLayoutBinding* bindingsData,
	VkDescriptorSetLayout* descriptorSetLayout
)
{
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = bindingsCount;
	layoutInfo.pBindings = bindingsData;

	VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo,
		nullptr, descriptorSetLayout);
	if (result != VK_SUCCESS) {
		// TODO PrintVkError(result);
		throw std::runtime_error("failed to create SkyBox descriptor set layout!");
	}
}

void cleanupDescriptorSetLayout(
	const VkDevice device,
	const VkDescriptorSetLayout layout
)
{
	vkDestroyDescriptorSetLayout(device, layout, nullptr);
}