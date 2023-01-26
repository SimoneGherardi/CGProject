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

class GLFWSurfaceFactory : public SurfaceFactory
{
private:
	GLFWwindow** _Window;
public:
	GLFWSurfaceFactory(GLFWwindow** window);
	VkResult CreateSurface(
		const VkInstance instance,
		VkSurfaceKHR* surface
	) override;
};