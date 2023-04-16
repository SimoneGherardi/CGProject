#pragma once
#include "defines.h"
#include "GlobalData.h"
#include "InstanceData.h"
#include "DeviceMemory.h"
#include "InstanceDataStore.h"

struct FrameData
{
	struct Global
	{
		GlobalData Data;
		Buffer Buffer;
		VkDescriptorSet DescriptorSet;

		void Update(const VulkanContext context);
	} Global;

	struct Objects
	{
		std::vector<VkDrawIndexedIndirectCommand> Commands;
		InstanceDataStore InstanceDataStore;
		Buffer CommandsBuffer;

		std::vector<GPUInstanceData> Data;
		Buffer Buffer;
		VkDescriptorSet DescriptorSet;

		void Update(const VulkanContext context);
	} Objects;
};