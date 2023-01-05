#pragma once
#include <string>
#include "Loader3D.h"
#include "ThreeDimensionalDataTypes.h"
enum Models {
};

std::string GetModelPath(Models model);
ModelType GetModelType(Models model);
std::string GetModelTexture(Models model);
Loader3D GetModelLoader(Models model);