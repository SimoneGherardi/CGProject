#pragma once

#include "Loader3D.h"
#include "common/asset_types.hpp"

void loadTextureFromFile(const char* fileName);

class GLTFLoader : Loader3D
{
public:
	void LoadMesh(const char* FName, ModelData& MD, VertexDescriptor& VD);
};