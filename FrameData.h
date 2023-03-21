#pragma once
#include "defines.h"
#include "GlobalData.h"
#include "MemoryReference.h"

struct FrameData
{
	struct Global
	{
		GlobalData Data;
		BufferMemoryReference MemoryReference;
		VkDescriptorSet DescriptorSet;
	} Global;
};