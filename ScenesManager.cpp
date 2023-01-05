#include "ScenesManager.h"
#include "PipelineBuilder.h"
#include "ObjLoader.h"
#include "GLTFLoader.h"
#include "assets.h"

void ScenesManager::LoadScene(Scene scene)
{
	static std::string basePath = "/resources";
	PipelineBuilder builder;

	// use iterator with for loop
	Loader3D* objLoader = new ObjLoader(basePath);
	Loader3D* gltfLoader = new GLTFLoader(basePath);
	for (auto iter = scene.ReferencedModels.begin(); iter != scene.ReferencedModels.end(); ++iter) {
		ModelData md;
		switch (GetModelType(*iter))
		{
		case ModelType::GLTF:
			objLoader->LoadMesh(GetModelPath(*iter), md)
		}
		builder.AddModel(iter);
	}
}