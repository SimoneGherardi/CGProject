#pragma once
#include <tiny_gltf.h>

#include "Loader3D.h"

class GLTFLoader : Loader3D
{
public:
	void LoadMesh(const char* FName, ModelData& MD, VertexDescriptor& VD);
};

