#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "ModelId.h"

struct GPUInstanceData
{
	glm::mat4 ModelMatrix;
	uint32_t ModelId;
	uint32_t TextureIndex;
	uint32_t _padding1;
	uint32_t _padding2;
};

//struct InstanceData
//{
//	GPUInstanceData GPUData;
//	ModelId ModelId;
//};