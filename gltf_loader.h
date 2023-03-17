#pragma once

#include "Loader3D.h"
#include "asset_types.hpp"

void loadDataFromGLTF(const char* fileName, std::vector<GLTFTexture>& allTextures, std::vector<GLTFMaterial>& allMaterial, std::vector<GLTFArmature>& allArmatures, std::vector<GLTFAnimation>& allAnimations);

class GLTFLoader : Loader3D
{
public:
	void LoadMesh(const char* FName, ModelData& MD, VertexDescriptor& VD);
};