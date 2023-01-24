#pragma once
#include "defines.h"

extern void initializeSurface(const VkInstance instance, GLFWwindow* window, VkSurfaceKHR* surface);

extern void cleanupSurface(const VkInstance instance, const VkSurfaceKHR surface);