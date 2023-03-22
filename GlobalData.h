#pragma once
#include "defines.h"
#include <glm/glm.hpp>

struct GlobalData
{
	struct Camera {
		glm::vec4 View;
		glm::vec4 Projection;
		glm::vec4 ViewProjection;
	};

	struct Sun {
		glm::vec3 Position;
		glm::vec3 Color;
	};

	glm::vec3 AmbientLight;
};