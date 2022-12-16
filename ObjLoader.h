#pragma once
#include "Loader3D.h"
#include <tiny_obj_loader.h>

class ObjLoader : Loader3D
{
public:
	void LoadMesh(const char* FName, ModelData& MD, VertexDescriptor& VD);
};

