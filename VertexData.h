#pragma once
#include "defines.h"
#include <glm/glm.hpp>


struct VertexData {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 UV;
	glm::vec3 Tangent;
	glm::vec4 Color;
	uint32_t WeightsOffset;
	uint32_t WeightsCount;
	uint32_t BonesOffset;
	uint32_t BonesCount;
};
