#pragma once
#include <map>
#include <vector>
#include "ModelId.h"
#include "InstanceData.h"
#include "defines.h"

struct BuiltInstanceData
{
	std::vector<VkDrawIndexedIndirectCommand> Commands;
	std::vector<GPUInstanceData> Data;
};

struct InstanceDataStore
{
	std::map<ModelId, std::vector<GPUInstanceData>> Data = {};

	void Add(const ModelId id, const GPUInstanceData data);
	void RemoveAll();

	BuiltInstanceData Build();
};