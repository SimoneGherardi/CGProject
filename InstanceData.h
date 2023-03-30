#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "ModelId.h"

struct GPUInstanceData
{
	glm::mat4 ModelMatrix;
};

struct InstanceData
{
	GPUInstanceData GPUData;
	ModelId ModelId;
};