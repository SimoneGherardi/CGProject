#pragma once

#include "Loader3D.h"

class GLTFLoader : Loader3D
{
public:
	Texture loadTextureFromFile(const char* fileName);
	void loadMesh(const char* fileName, ModelData& MD, VertexDescriptor& VD);
};
