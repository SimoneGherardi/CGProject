#pragma once

#include "Loader3D.h"
#include "asset_types.hpp"

extern void loadDataFromGLTF(const char* fileName);
extern GLTFModel loadModelFromBin(std::string fileName);
extern GLTFPrimitive loadPrimitiveFromBin(std::string fileName);
extern GLTFMaterial loadMaterialFromBin(std::string fileName);

class GLTFLoader : Loader3D
{
public:
	void LoadMesh(const char* FName, ModelData& MD, VertexDescriptor& VD);
};