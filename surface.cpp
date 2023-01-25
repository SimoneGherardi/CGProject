#include "surface.h"

void initializeSurface(
    const VkInstance instance,
    VkSurfaceKHR* surface,
    SurfaceFactory* factory
)
{
    CheckVkResult(factory->CreateSurface(instance, surface));
}

void cleanupSurface(const VkInstance instance, const VkSurfaceKHR surface)
{
    vkDestroySurfaceKHR(instance, surface, nullptr);
}