#pragma once
#include "defines.h"

class SurfaceFactory
{
public:
	virtual VkResult CreateSurface(
		const VkInstance instance,
		VkSurfaceKHR* surface
	) = 0;
};