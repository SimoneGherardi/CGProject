#pragma once
#include "ThreeDimensionalDataTypes.h"
#include "vulkan.h"

class PipelineBuilder
{
public:
	void AddModel(const Model& m);
	void Build(CGProject& vulkan);
};
