#pragma once
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "VertexData.h"
#include "ModelData.h"
#include "asset_types.hpp"
#include "ModelId.h"
#include "Models.h"
#include "HostLocalAllocator.h"
#include "MemoryReference.h"
#include "InstanceData.h"

struct RenderContext
{
	std::vector<VertexData> Vertices = {};
	std::vector<uint16_t> Indices = {};

	std::map<ModelId, BakedModelInfo> Models = {};

	HostLocalAllocator* Allocator = nullptr;
	HostLocalMemoryReference VertexMemoryReference = {};
	HostLocalMemoryReference IndexMemoryReference = {};

	BakedModelInfo BakeModel(const ModelId id, std::vector<GLTFModel> models, const std::vector<GLTFPrimitive> primitives, const std::vector<GLTFMaterial> materials);

	void BuildAssets();
	void Initialize(const VulkanContext context);
	void Cleanup(const VulkanContext context);

	void QueueDraw(VkCommandBuffer cmd, std::vector<InstanceData> instances);
	void DrawModel(VkCommandBuffer commandBuffer, const ModelId id);

	static RenderContext& GetInstance();
};