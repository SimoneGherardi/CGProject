#pragma once
#include "defines.h"
#include <glm/glm.hpp>

struct GlobalData
{
	glm::mat4 CameraView;
	glm::mat4 CameraProjection;
	glm::mat4 CameraViewProjection;

	glm::vec3 SunDirection;
	glm::vec3 SunColor;

	glm::vec3 AmbientLight;
};