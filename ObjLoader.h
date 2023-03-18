#pragma once
#include "Loader3D.h"

class ObjLoader : Loader3D
{
public:
	void LoadMesh(const char* FName, ModelData& MD, VertexDescriptor& VD);
};

