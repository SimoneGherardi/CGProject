#include "tests.h"
#include "descriptor_set.h"
#include <glm/glm.hpp>

float timer = 0;

glm::vec3 SUN_DIRECTION = glm::normalize(glm::vec3(1,-2,-1));
glm::vec3 AMBIENT_LIGHT = glm::normalize(glm::vec3(0.3f,0.3f,0.3f));

void TEST_CAMERA(const VulkanContext context, const float width, const float height, float delta, const VkCommandBuffer cmd, const VkPipelineLayout layout, FrameData* frameData)
{
	//timer += delta;

	float xl = glm::sin(timer); // change 0 for spinning
	float zl = glm::cos(timer); // change 0 for spinning

	glm::vec3 camPos = { 0, 0, -16 };
	glm::vec3 camRotAxis = { 0, 1.0f, 0 };

	glm::mat4 view = glm::translate(glm::mat4(1.0f), camPos);
	glm::mat4 projection = glm::perspective(glm::radians(70.f), width / height, 0.1f, 200.0f);
	projection[1][1] *= -1;

	GlobalData* data = &(frameData->Global.Data);

	//data->CameraProjection = projection;
	//data->CameraView = view;
	//data->CameraViewProjection = projection * view;

	data->SunDirection = SUN_DIRECTION;
	data->AmbientLight = AMBIENT_LIGHT;
	frameData->Global.Update(context);

	vkCmdBindDescriptorSets(
		cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &(frameData->Global.DescriptorSet), 0, nullptr
	);

	GPUInstanceData d = {};
	d.ModelMatrix = glm::rotate(timer, glm::vec3(0,1,0));
	// frameData->Objects.InstanceDataStore.Add(Models::SUZANNE, d);
	frameData->Objects.Update(context);

	updateDescriptorSet(
		context,
		frameData->Objects.Buffer.Buffer,
		frameData->Objects.Buffer.Size,
		0,
		frameData->Objects.DescriptorSet,
		1,
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
	);

	vkCmdBindDescriptorSets(
		cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 1, 1, &(frameData->Objects.DescriptorSet), 0, nullptr
	);
}
