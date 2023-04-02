#include "RenderContext.h"
#include "gltf_loader.h"
#include <filesystem>
#include "MemoryTransferer.h"

#define PACK_VEC3(v) glm::vec3(v[0], v[1], v[2])
#define PACK_VEC4(v) glm::vec4(v[0], v[1], v[2], v[3])

#define ASSET_PATH "resources/models/gltf/"

BakedModelInfo RenderContext::_BakeModel(const ModelId id, const std::vector<GLTFModel> models, const std::vector<GLTFPrimitive> primitives, const std::vector<GLTFMaterial> materials)
{
	BakedModelInfo modelInfo;
	modelInfo.Id = id;
	for (auto model : models)
	{
		for (auto primitiveId : model.Primitives)
		{
			auto primitive = primitives[primitiveId];
			auto material = materials[primitive.MaterialId];

			BakedMeshInfo meshInfo;
			meshInfo.VerticesOffset = Vertices.size();
			meshInfo.IndicesOffset = Indices.size();
			meshInfo.IndicesCount = primitive.Indices.size();
			for (auto i = 0; i < primitive.Positions.size(); i++)
			{
				VertexData vertex = {};
				vertex.Position = PACK_VEC3(primitive.Positions[i]);
				vertex.Normal = PACK_VEC3(primitive.Normals[i]);
				vertex.Color = PACK_VEC4(material.BaseColorFactor);
				Vertices.push_back(vertex);
			}

			for (auto index : primitive.Indices)
			{
				Indices.push_back(index);
			}

			modelInfo.Meshes.push_back(meshInfo);
		}
	}
	Models[id] = modelInfo;
	return modelInfo;
}

void RenderContext::BuildAssets()
{
	for (auto m : Models::ModelsToLoad)
	{
		loadDataFromGLTF((ASSET_PATH + m.Name + ".gltf").c_str());
	}
}

void RenderContext::Initialize(const VulkanContext context, const Buffer stagingBuffer, DeviceMemory* memory)
{
	if (IsInitialized)
	{
		throw std::runtime_error("RenderContext already initialized");
	}
	for (auto m : Models::ModelsToLoad)
	{
		std::vector<GLTFPrimitive> primitives = {};
		std::vector<GLTFMaterial> materials = {};
		std::vector<GLTFModel> models = {};
		std::string materialsPath = ".\\resources\\models\\gltf\\" + m.Name + "\\GLTFMaterial";
		std::string primitivesPath = ".\\resources\\models\\gltf\\" + m.Name + "\\GLTFPrimitive";
		std::string modelsPath = ".\\resources\\models\\gltf\\" + m.Name + "\\GLTFModel";
		for (const auto& entry : std::filesystem::directory_iterator(materialsPath)) {
			materials.push_back(loadMaterialFromBin(entry.path().string()));
		}
		for (const auto& entry : std::filesystem::directory_iterator(primitivesPath)) {
			primitives.push_back(loadPrimitiveFromBin(entry.path().string()));
		}
		for (const auto& entry : std::filesystem::directory_iterator(modelsPath)) {
			models.push_back(loadModelFromBin (entry.path().string()));
		}

		_BakeModel(m.Id, models, primitives, materials);
	}

	auto vsize = Vertices.size() * sizeof(VertexData);
	auto isize = Indices.size() * sizeof(uint16_t);
	VertexBuffer = memory->NewBuffer(vsize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	IndexBuffer = memory->NewBuffer(isize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

	MemoryTransferer m = MemoryTransferer(context, VertexBuffer, Vertices.data(), vsize);
	m.TransferStaged(stagingBuffer, 0);
	m = MemoryTransferer(context, IndexBuffer, Indices.data(), isize);
	m.TransferStaged(stagingBuffer, 0);
	Memory = memory;
	IsInitialized = true;
}


void RenderContext::Cleanup(const VulkanContext context)
{
	if (!IsInitialized)
	{
		throw std::runtime_error("RenderContext not initialized");
	}
	Memory->FreeBuffer(VertexBuffer);
	Memory->FreeBuffer(IndexBuffer);
}

RenderContext& RenderContext::GetInstance()
{
	static RenderContext instance;
	return instance;
}