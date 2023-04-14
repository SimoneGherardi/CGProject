#pragma once
#include <vector>
#include "VertexData.h"
#include <glm/glm.hpp>
#include <stdint.h>
#include "ModelId.h"


struct BakedMeshInfo
{
	uint32_t VerticesOffset;
	uint32_t IndicesOffset;
	uint32_t IndicesCount;
};

struct BakedModelInfo
{
	ModelId Id;
	std::vector<BakedMeshInfo> Meshes;
	uint32_t TextureId;
};