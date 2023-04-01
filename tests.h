#pragma once
#include "defines.h"
#include "VertexData.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "FrameData.h"
#include "gltf_loader.h"
#include "asset_types.hpp"
#include <filesystem>
#include "RenderContext.h"

extern void TEST_CAMERA(const VulkanContext context, const float width, const float height, float delta, const VkCommandBuffer cmd, const VkPipelineLayout layout, FrameData* frameData);

extern void TEST_RENDER(const VkCommandBuffer cmd, const VkPipelineLayout layout, FrameData* data);