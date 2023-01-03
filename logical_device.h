#pragma once
#include "defines.h"
#include "queue_families.h"
#include "extensions.h"
#include "validation_layers.h"
#include "swap_chain_support.h"

extern VkDevice initializeLogicalDevice(
    const VkPhysicalDevice physicalDevice,
    const VkSurfaceKHR surface,
    const ValidationLayers validationLayers,
    const Extensions deviceExtensions,
    VkDevice* device,
    VkQueue* graphicsQueue,
    VkQueue* presentationQueue
);

extern void cleanupLogicalDevice(const VkDevice device);