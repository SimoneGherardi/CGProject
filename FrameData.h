#pragma once
#include "defines.h"
#include "GlobalData.h"
#include "MemoryReference.h"
#include "InstanceData.h"

struct FrameData
{
	struct Global
	{
		GlobalData Data;
		HostLocalMemoryReference MemoryReference;
		VkDescriptorSet DescriptorSet;
	} Global;

	struct Objects
	{
		std::vector<InstanceData> Data;
		HostLocalMemoryReference MemoryReference;
		VkDescriptorSet DescriptorSet;
	} Objects;
};