#include "RenderContext.h"
#include "gltf_loader.h"
#include <filesystem>
#include "MemoryTransferer.h"
#include "image.h"
#include "image_views.h"
#include "ImmediateCommandBuffer.h"
#include "VulkanStructs.h"

#define PACK_VEC2(v) glm::vec2(v[0], v[1])
#define PACK_VEC3(v) glm::vec3(v[0], v[1], v[2])
#define PACK_VEC4(v) glm::vec4(v[0], v[1], v[2], v[3])

#define ASSET_PATH "resources/models/gltf/"

std::set<std::string> _GetOrderedFilesFromDirectory(std::string directory)
{
	std::set<std::string> toret = {};
	if (!std::filesystem::exists(directory)) {
		return toret;
	}
	for (const auto& entry : std::filesystem::directory_iterator(directory)) {
		toret.insert(entry.path().string());
	}
	return toret;
}



// TODO make a memory transferer
void copyPixelsToImage(VulkanContext context, GLTFTexture texture, DeviceMemory* stagingMemory, VkImage image)
{
	ImmediateCommandBuffer immediate = ImmediateCommandBuffer(context);
	auto width = static_cast<uint32_t>(texture.Width);
	auto height = static_cast<uint32_t>(texture.Height);
	auto size = texture.Pixels.size();

	Buffer stagingBuffer = stagingMemory->NewBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	MemoryTransferer m = MemoryTransferer(context, stagingBuffer, texture.Pixels.data(), size);
	m.TransferMapped();

	immediate.Submit([=](VkCommandBuffer cmd) {
		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };


		vkCmdCopyBufferToImage(
			cmd,
			stagingBuffer.Buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);
	});
	immediate.Wait();
	stagingMemory->FreeBuffer(stagingBuffer);
}


// TODO move in a library
void transitionImageLayout(VulkanContext context, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	ImmediateCommandBuffer immediate = ImmediateCommandBuffer(context);
	immediate.Submit([=](VkCommandBuffer cmd) {
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;

		//these are used to transfer queue ownership, which we aren't doing
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			//Unsupported layout transition
			throw new std::runtime_error("Attempting an unsupported image layout transition");
		}

		vkCmdPipelineBarrier(
			cmd,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	});
	immediate.Wait();
}

Texture createTexture(
	const VulkanContext context,
	const GLTFTexture source,
	DeviceMemory* stagingMemory,
	DeviceMemory* memory
)
{
	Texture t = {};
	t.ImageItem = memory->NewImage(
		source.Width,
		source.Height,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
	);
	transitionImageLayout(context, t.ImageItem.Image, t.ImageItem.Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyPixelsToImage(context, source, stagingMemory, t.ImageItem.Image);
	transitionImageLayout(context, t.ImageItem.Image, t.ImageItem.Format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	t.View = initializeImageView(
		context->Device,
		t.ImageItem.Image,
		t.ImageItem.Format,
		VK_IMAGE_ASPECT_COLOR_BIT,
		1,
		VK_IMAGE_VIEW_TYPE_2D,
		4
	);
	return t;
}






BakedModelInfo RenderContext::_BakeModel(
	const VulkanContext context,
	const ModelId id,
	const std::vector<GLTFModel> models,
	const std::vector<GLTFPrimitive> primitives,
	const std::vector<GLTFMaterial> materials,
	const std::vector<GLTFTexture> textures
)
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
				if(primitive.UVCoordinatesNum > 0) vertex.UV = PACK_VEC2(primitive.UVCoordinates[i]);
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
		LOGDBG("Loading model " << m.Name);
		loadDataFromGLTF((ASSET_PATH + m.Name + ".gltf").c_str());
	}
}

void RenderContext::Initialize(const VulkanContext context, DeviceMemory* stagingMemory, DeviceMemory* memory)
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
		std::vector<GLTFTexture> textures = {};
		std::string materialsPath = "./resources/models/gltf/" + m.Name + "/GLTFMaterial";
		std::string primitivesPath = "./resources/models/gltf/" + m.Name + "/GLTFPrimitive";
		std::string modelsPath = "./resources/models/gltf/" + m.Name + "/GLTFModel";
		std::string texturesPath = "./resources/models/gltf/" + m.Name + "/GLTFTexture";

		auto materialFiles = _GetOrderedFilesFromDirectory(materialsPath);
		auto primitiveFiles = _GetOrderedFilesFromDirectory(primitivesPath);
		auto modelFiles = _GetOrderedFilesFromDirectory(modelsPath);
		auto textureFiles = _GetOrderedFilesFromDirectory(texturesPath);
		for (const auto& m : materialFiles) {
			materials.push_back(loadMaterialFromBin(m));
		}
		for (const auto& p : primitiveFiles) {
			primitives.push_back(loadPrimitiveFromBin(p));
		}
		for (const auto& m : modelFiles) {
			models.push_back(loadModelFromBin(m));
		}
		for (const auto& t : textureFiles) {
			textures.push_back(loadTextureFromBin(t));
		}

		_BakeModel(context, m.Id, models, primitives, materials, textures);
		Models[m.Id].TextureId = 0xFFFFFFFF; // no texture

		if (textures.size() > 0)
		{
			auto& source = textures[0]; // load only first texture... for now. TODO
			Texture t = createTexture(context, source, stagingMemory, memory);
			Models[m.Id].TextureId = Textures.size();
			Textures.push_back(t);
		}
	}

	auto vsize = Vertices.size() * sizeof(VertexData);
	auto isize = Indices.size() * sizeof(uint16_t);
	VertexBuffer = memory->NewBuffer(vsize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	IndexBuffer = memory->NewBuffer(isize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

	auto stagingBuffer = stagingMemory->NewBuffer(1024 * 1024, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	MemoryTransferer m = MemoryTransferer(context, VertexBuffer, Vertices.data(), vsize);
	m.TransferStaged(stagingBuffer, 0);
	m = MemoryTransferer(context, IndexBuffer, Indices.data(), isize);
	m.TransferStaged(stagingBuffer, 0);
	stagingMemory->FreeBuffer(stagingBuffer);

	for (auto& t : Textures)
	{
		VkDescriptorImageInfo descriptorImageInfo = {};
		descriptorImageInfo.sampler = nullptr;
		descriptorImageInfo.imageView = t.View;
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		TexturesImageInfos.push_back(descriptorImageInfo);
	}

	VkSamplerCreateInfo samplerCreateInfo = VulkanStructs::SamplerCreateInfo(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	CheckVkResult(vkCreateSampler(context->Device, &samplerCreateInfo, nullptr, &TextureSampler));

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