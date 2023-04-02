#include "descriptor_set.h"

void initializeDescriptorSet(
	const VulkanContext context,
	const VkDescriptorPool pool,
	const uint32_t layoutsCount,
	const VkDescriptorSetLayout* layouts,
	VkDescriptorSet* descriptorSet
)
{
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.pNext = nullptr;
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool;
	allocInfo.descriptorSetCount = layoutsCount;
	allocInfo.pSetLayouts = layouts;
	vkAllocateDescriptorSets(context->Device, &allocInfo, descriptorSet);
}

void updateDescriptorSet(
	const VulkanContext context,
	const VkBuffer buffer,
	const VkDeviceSize size,
	const uint32_t dstBinding,
	const VkDescriptorSet dstSet,
	const uint32_t descriptorCount,
	const VkDescriptorType descriptorType
)
{
	VkDescriptorBufferInfo binfo;
	binfo.buffer = buffer;
	binfo.offset = 0;
	binfo.range = size;

	VkWriteDescriptorSet setWrite = {};
	setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	setWrite.pNext = nullptr;
	setWrite.dstBinding = dstBinding;
	setWrite.dstSet = dstSet;

	setWrite.descriptorCount = descriptorCount;
	setWrite.descriptorType = descriptorType;
	setWrite.pBufferInfo = &binfo;


	vkUpdateDescriptorSets(context->Device, 1, &setWrite, 0, nullptr);
}