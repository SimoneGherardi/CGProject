#pragma once
#include "vertex_buffers.h"

VkVertexInputBindingDescription Vertex::getBindingDescription(const uint32_t binding, const uint32_t stride, const VkVertexInputRate inputRate) {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = binding;
    bindingDescription.stride = stride;
    bindingDescription.inputRate = inputRate;
    return bindingDescription;
}

VkVertexInputBindingDescription Vertex::getBindingDescription(const uint32_t binding, const uint32_t stride) {
    return Vertex::getBindingDescription(binding, stride, VK_VERTEX_INPUT_RATE_VERTEX);
}

VkVertexInputBindingDescription Vertex::getBindingDescription(const uint32_t binding) {
    return Vertex::getBindingDescription(binding, sizeof(Vertex));
}

VkVertexInputBindingDescription Vertex::getBindingDescription() {
    return Vertex::getBindingDescription(0);
}

VkVertexInputAttributeDescription Vertex::getVertexInputAttributeDescription(const uint32_t binding, const uint32_t location, const VkFormat format, const uint32_t offset) {
    VkVertexInputAttributeDescription d = {};
    d.binding = binding;
    d.location = location;
    d.format = format;
    d.offset = offset;
    return d;
}

VertexAttributeDescriptions Vertex::getAttributeDescriptions() {
    auto attr1 = Vertex::getVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, pos));
    auto attr2 = Vertex::getVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos));
    VertexAttributeDescriptions attributeDescriptions = { attr1, attr2 };
    return attributeDescriptions;
}

void createVertexBuffer(const VkDevice device, const std::vector<Vertex> vertices, VkBuffer* vertexBuffer)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, vertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }
}

void cleanupVertexBuffer(const VkDevice device, const VkBuffer vertexBuffer)
{
    vkDestroyBuffer(device, vertexBuffer, nullptr);
}