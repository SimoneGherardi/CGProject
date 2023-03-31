#pragma once
#include "defines.h"
#include "GlobalData.h"
#include "MemoryReference.h"
#include "InstanceData.h"
#include "DeviceMemory.h"

struct FrameData
{
	struct Global
	{
		GlobalData Data;
		// HostLocalMemoryReference MemoryReference;
		Buffer Buffer;
		VkDescriptorSet DescriptorSet;

		void Update(const VulkanContext context);
	} Global;

	struct Objects
	{
		std::vector<InstanceData> Data;
		// HostLocalMemoryReference MemoryReference;
		Buffer Buffer;
		VkDescriptorSet DescriptorSet;

		void Update(const VulkanContext context);
	} Objects;
};