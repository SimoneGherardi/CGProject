#pragma once
#include "defines.h"
#include "queue_families.h"

extern void initializeCommandPool(
	const VkDevice device,
	const QueueFamilyIndices queueFamilyIndices,
	VkCommandPool* commandPool
);

extern void cleanupCommandPool(
	const VkDevice device,
	const VkCommandPool commandPool
);