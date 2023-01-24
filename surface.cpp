#include "surface.h"

void initializeSurface(const VkInstance instance, GLFWwindow* window, VkSurfaceKHR* surface) {
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

void cleanupSurface(const VkInstance instance, const VkSurfaceKHR surface)
{
    vkDestroySurfaceKHR(instance, surface, nullptr);
}