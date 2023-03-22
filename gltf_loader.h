#pragma once

#include "Loader3D.h"
#include "asset_types.hpp"

void loadDataFromGLTF(const char* fileName);

class GLTFLoader : Loader3D
{
public:
	void LoadMesh(const char* FName, ModelData& MD, VertexDescriptor& VD);
};