#pragma once
#include "defines.h"
#include "VertexData.h"
#include "Allocator.h"
#include "MemoryReference.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "FrameData.h"

std::vector<VertexData> test_vertices;
Allocator* test_allocator;
BufferMemoryReference test_reference;

void TEST_INIT(const VulkanContext context)
{
	test_allocator = new Allocator(context, 1024, false);
	test_vertices.resize(3);
	test_vertices[0].Position = { 1.0f, 1.0f, 0.0f };
	test_vertices[1].Position = { -1.0f, 1.0f, 0.0f };
	test_vertices[2].Position = { 0.0f, -1.0f, 0.0f };

	test_vertices[0].Color = { 0.0f, 0.0f, 1.0f, 1.0f };
	test_vertices[1].Color = { 0.0f, 1.0f, 0.0f, 1.0f };
	test_vertices[2].Color = { 1.0f, 0.0f, 0.0f, 1.0f };

	test_reference = test_allocator->AllocateAndSet(
		test_vertices.data(),
		test_vertices.size() * sizeof(VertexData),
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
	);
}

float timer = 0;
void TEST_CAMERA(const float width, const float height, float delta, const VkCommandBuffer cmd, const VkPipelineLayout layout, Allocator* frameDataAllocator, FrameData* frameData)
{
	timer += delta;
	float x = glm::sin(timer) * 4;
	float y = glm::cos(timer) * 4;
	glm::vec3 camPos = { x, y, -10.0f };

	glm::mat4 view = glm::translate(glm::mat4(1.0f), camPos);
	glm::mat4 projection = glm::perspective(glm::radians(70.f), width / height, 0.1f, 200.0f);
	projection[1][1] *= -1;

	GlobalData* data = &(frameData->Global.Data);

	data->CameraProjection = projection;
	data->CameraView = view;
	data->CameraViewProjection = projection * view;

	frameDataAllocator->TransferFromHost(data, 0, sizeof(GlobalData));

	vkCmdBindDescriptorSets(
		cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &(frameData->Global.DescriptorSet), 0, nullptr
	);
}

void TEST_CLEANUP()
{
	test_allocator->Cleanup();
}

void TEST_RENDER(const VkCommandBuffer cmd)
{
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(cmd, 0, 1, &(test_reference.Buffer), &offset);
	vkCmdDraw(cmd, test_vertices.size(), 1, 0, 0);
}