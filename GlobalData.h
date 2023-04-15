#pragma once
#include "defines.h"
#include <glm/glm.hpp>

/*
    Scalars have to be aligned by N (= 4 bytes given 32 bit floats).
    Vec2 must be aligned by 2N (= 8 bytes)
    Vec3 or vec4 must be aligned by 4N (= 16 bytes)
    A nested structure must be aligned by the base alignment of its members rounded up to a multiple of 16.
    Mat4 matrix must have the same alignment as a vec4.
	https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap15.html#interfaces-resources-layout
*/

struct GlobalData
{
	glm::mat4 CameraView;
	glm::mat4 CameraProjection;
	glm::mat4 CameraViewProjection;
	glm::vec3 CameraPosition;
	uint8_t _pad0;

	glm::vec3 SunDirection;
	uint8_t _pad1;
	glm::vec3 SunColor;
	uint8_t _pad2;

	glm::vec3 AmbientLight;
	uint8_t _pad3;
};