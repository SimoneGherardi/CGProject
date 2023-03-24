#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct ModelData
{
	glm::vec3 Position;
	glm::quat Rotation;
	glm::vec3 Scale;
};