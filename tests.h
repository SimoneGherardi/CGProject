#pragma once
#include "defines.h"
#include "VertexData.h"
#include "Allocator.h"
#include "MemoryReference.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "FrameData.h"
#include "gltf_loader.h"
#include "asset_types.hpp"
#include <filesystem>
#include "HostLocalAllocator.h"
#include "RenderContext.h"

float timer = 0;

void TEST_CAMERA(const VulkanContext context, const float width, const float height, float delta, const VkCommandBuffer cmd, const VkPipelineLayout layout, FrameData* frameData)
{
	timer += delta;

	float xl = glm::sin(timer); // change 0 for spinning
	float zl = glm::cos(timer); // change 0 for spinning

	glm::vec3 camPos = { 0, 0, -3};
	glm::vec3 camRotAxis = { 0, 1.0f, 0 };

	glm::mat4 view = glm::translate(glm::mat4(1.0f), camPos);
	glm::mat4 projection = glm::perspective(glm::radians(70.f), width / height, 0.1f, 200.0f);
	projection[1][1] *= -1;

	GlobalData* data = &(frameData->Global.Data);

	data->CameraProjection = projection;
	data->CameraView = view;
	data->CameraViewProjection = projection * view;

	data->SunDirection = { xl, 0.0f, zl };
	//frameData->Global.MemoryReference.Transfer();
	frameData->Global.Update(context);

	vkCmdBindDescriptorSets(
		cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &(frameData->Global.DescriptorSet), 0, nullptr
	);

	frameData->Objects.Data[0].GPUData.ModelMatrix = glm::rotate(timer, glm::vec3(0,0,1));
	// frameData->Objects.MemoryReference.Transfer();
	frameData->Objects.Update(context);

	vkCmdBindDescriptorSets(
		cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 1, 1, &(frameData->Objects.DescriptorSet), 0, nullptr
	);
}

void TEST_RENDER(const VkCommandBuffer cmd, const VkPipelineLayout layout, FrameData* data)
{
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(cmd, 0, 1, &(RenderContext::GetInstance().VertexMemoryReference.Buffer), &offset);
	vkCmdBindIndexBuffer(cmd, RenderContext::GetInstance().IndexMemoryReference.Buffer, 0, VK_INDEX_TYPE_UINT16);
	RenderContext::GetInstance().QueueDraw(cmd, data->Objects.Data);
}