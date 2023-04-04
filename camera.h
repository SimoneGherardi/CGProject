#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

struct Camera {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};