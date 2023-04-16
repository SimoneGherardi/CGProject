#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "ModelId.h"

/*
    Scalars have to be aligned by N (= 4 bytes given 32 bit floats).
    Vec2 must be aligned by 2N (= 8 bytes)
    Vec3 or vec4 must be aligned by 4N (= 16 bytes)
    A nested structure must be aligned by the base alignment of its members rounded up to a multiple of 16.
    Mat4 matrix must have the same alignment as a vec4.
	https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap15.html#interfaces-resources-layout
*/

struct GPUInstanceData
{
	glm::mat4 ModelMatrix;
	uint32_t ModelId;
	uint32_t TextureIndex;
	uint32_t _padding1;
	uint32_t _padding2;
};