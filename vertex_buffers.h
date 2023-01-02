#pragma once
#include "defines.h"
#include "glm/glm.hpp"

typedef std::array<VkVertexInputAttributeDescription, 2> VertexAttributeDescriptions;

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription(const uint32_t binding, const uint32_t stride, const VkVertexInputRate inputRate);
    static VkVertexInputBindingDescription getBindingDescription(const uint32_t binding, const uint32_t stride);
    static VkVertexInputBindingDescription getBindingDescription(const uint32_t binding);
    static VkVertexInputBindingDescription getBindingDescription();

    static VkVertexInputAttributeDescription getVertexInputAttributeDescription(const uint32_t binding, const uint32_t location, const VkFormat format, const uint32_t offset);

    static VertexAttributeDescriptions getAttributeDescriptions();
};

void createVertexBuffer(const VkDevice device, const std::vector<Vertex> vertices, VkBuffer* vertexBuffer);
void cleanupVertexBuffer(const VkDevice device, const VkBuffer vertexBuffer);