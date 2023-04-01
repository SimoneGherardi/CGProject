#pragma once
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "VertexData.h"
#include "ModelData.h"
#include "asset_types.hpp"
#include "ModelId.h"
#include "Models.h"
#include "InstanceData.h"
#include "DeviceMemory.h"

struct RenderContext
{
	bool IsInitialized = false;

	DeviceMemory* Memory;

	std::vector<VertexData> Vertices = {};
	std::vector<uint16_t> Indices = {};

	std::map<ModelId, BakedModelInfo> Models = {};

	Buffer VertexBuffer = {};
	Buffer IndexBuffer = {};

	void BuildAssets();
	void Initialize(const VulkanContext context, const Buffer stagingBuffer, DeviceMemory* memory);
	void Cleanup(const VulkanContext context);

	void QueueDraw(VkCommandBuffer cmd, std::vector<InstanceData> instances);
	void DrawModel(VkCommandBuffer commandBuffer, const ModelId id);

	static RenderContext& GetInstance();

private:
	BakedModelInfo _BakeModel(const ModelId id, std::vector<GLTFModel> models, const std::vector<GLTFPrimitive> primitives, const std::vector<GLTFMaterial> materials);
};