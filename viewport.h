#pragma once
#include "defines.h"

extern VkViewport getViewport(const float x, const float y, const float width, const float height, const float minDepth, const float maxDepth);
extern VkViewport getViewport(const float x, const float y, const VkExtent2D& extent, const float minDepth, const float maxDepth);

extern VkRect2D getScissor(const float offx, const float offy, const VkExtent2D& extent);