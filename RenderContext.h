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

struct Texture
{
	Image ImageItem;
	VkImageView View;
};

struct RenderContext
{
	bool IsInitialized = false;

	DeviceMemory* Memory;

	std::vector<VertexData> Vertices = {};
	std::vector<uint16_t> Indices = {};
	std::vector<Texture> Textures = {};
	std::vector<VkDescriptorImageInfo> TexturesImageInfos = {};

	VkSampler TextureSampler;

	std::map<ModelId, BakedModelInfo> Models = {};

	Buffer VertexBuffer = {};
	Buffer IndexBuffer = {};

	void BuildAssets();
	void Initialize(const VulkanContext context, DeviceMemory* stagingMemory, DeviceMemory* memory);
	void Cleanup(const VulkanContext context);

	static RenderContext& GetInstance();

private:
	BakedModelInfo _BakeModel(
		const VulkanContext context,
		const ModelId id,
		const std::vector<GLTFModel> models,
		const std::vector<GLTFPrimitive> primitives,
		const std::vector<GLTFMaterial> materials,
		const std::vector<GLTFTexture> textures
	);
};