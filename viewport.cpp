#include "viewport.h"

VkViewport getViewport(const float x, const float y, const float width, const float height, const float minDepth, const float maxDepth) {
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = width;
	viewport.height = height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	return viewport;
}

VkViewport getViewport(const float x, const float y, const VkExtent2D& extent, const float minDepth, const float maxDepth)
{
	float width = (float)extent.width;
	float height = (float)extent.height;
	return getViewport(x, y, width, height, minDepth, maxDepth);
}

VkRect2D getScissor(const float offx, const float offy, const VkExtent2D& extent)
{
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = extent;
	return scissor;
}