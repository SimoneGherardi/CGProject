#pragma once

#include "Loader3D.h"
#include "asset_types.hpp"

extern GLTFArmature loadArmatureFromBin(std::string fileName);
extern GLTFTexture loadTextureFromBin(std::string fileName);
extern GLTFMaterial loadMaterialFromBin(std::string fileName);
extern GLTFAnimation loadAnimationFromBin(std::string fileName);
extern GLTFAnimationChannel loadAnimationChannelFromBin(std::string fileName);
extern GLTFPrimitive loadPrimitiveFromBin(std::string fileName);
extern GLTFModel loadModelFromBin(std::string fileName);

extern void loadDataFromGLTF(const char* fileName);

class GLTFLoader : Loader3D
{
public:
	void LoadMesh(const char* FName, ModelData& MD, VertexDescriptor& VD);
};