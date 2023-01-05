#include "GLTFLoader.h"
#include "ObjLoader.h"

#include "assets.h"

std::string GetModelPath(Models model)
{
	switch (model)
	{
	default:
		return "";
	}
}
ModelType GetModelType(Models model)
{
	switch (model)
	{
	default:
		return ModelType::OBJ;
	}
}
std::string GetModelTexture(Models model)
{
	switch (model)
	{
	default:
		return "";
	}
}
Loader3D* GetModelLoader(Models model, std::string basePath)
{
	ModelType t = GetModelType(model);
	switch (t)
	{
	case ModelType::GLTF:
		return new GLTFLoader(basePath);
	case ModelType::OBJ:
		return new ObjLoader(basePath);
	}
}