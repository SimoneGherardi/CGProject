#pragma once
#include <tiny_gltf.h>

#include "Loader3D.h"

class GLTFLoader : public Loader3D
{
public:
	using Loader3D::Loader3D;
	void LoadMesh(const char* FName, ModelData& modelData);
};

