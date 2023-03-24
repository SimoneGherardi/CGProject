#pragma once
#include "defines.h"
#include "GlobalData.h"
#include "MemoryReference.h"

struct FrameData
{
	struct Global
	{
		GlobalData Data;
		HostLocalMemoryReference MemoryReference;
		VkDescriptorSet DescriptorSet;
	} Global;
};