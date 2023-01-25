#pragma once
#include "defines.h"
#include "surface_factory.h"

extern void initializeSurface(
    const VkInstance instance,
    VkSurfaceKHR* surface,
    SurfaceFactory* factory
);

extern void cleanupSurface(const VkInstance instance, const VkSurfaceKHR surface);