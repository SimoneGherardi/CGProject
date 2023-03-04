#pragma once

#include "Loader3D.h"
#include "asset_types.hpp"

void loadDataFromGLTF(const char* fileName, std::vector<Texture>& allTextures, std::vector<Material>& allMaterial, std::vector<Armature>& allArmatures, std::vector<Animation>& allAnimations);

class GLTFLoader : Loader3D
{
public:
	void LoadMesh(const char* FName, ModelData& MD, VertexDescriptor& VD);
};