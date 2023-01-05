#pragma once
#include "Loader3D.h"
#include <tiny_obj_loader.h>

class ObjLoader : public Loader3D
{
public:
	using Loader3D::Loader3D;
	void LoadMesh(const char* FName, ModelData& modelData);
};

