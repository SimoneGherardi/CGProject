#pragma once
#include <string>

#include "ThreeDimensionalDataTypes.h"

class Loader3D
{
protected:
	std::string baseBath;
public:
	Loader3D(std::string basePath);
	virtual void LoadMesh(const char* FName, ModelInfo& MD, VertexDescriptor& VD) = 0;
};