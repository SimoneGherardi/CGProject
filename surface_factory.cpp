#include "surface_factory.h"

GLFWSurfaceFactory::GLFWSurfaceFactory(GLFWwindow* window)
{
	_Window = window;
}

VkResult GLFWSurfaceFactory::CreateSurface(
	const VkInstance instance,
	VkSurfaceKHR* surface
)
{
	return glfwCreateWindowSurface(instance, _Window, nullptr, surface);
}