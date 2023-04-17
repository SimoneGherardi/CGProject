#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#define NEAR_PLANE 0.1f
#define FAR_PLANE 1000.0f

#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include "flecs.h"
#include "reactphysics3d/reactphysics3d.h"

class CameraInfos
{
public:
	// Stores the main vectors of the camera	

	float FOVDeg;
	// Stores the width and height of the window
	int Width;
	int Height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float speed = 0.1f;
	float sensitivityRotation = 10.0f;
	float sensitivityTranslation = 25.0f;
	float sensitivityScroll = 0.5f;

	// Camera constructor to set up initial values
	CameraInfos(int width, int height, float FOVDeg, glm::vec3 position);

	flecs::entity CameraEntity;
	glm::vec3 Position();
	glm::mat4 ProjectionMatrix();
	glm::mat4 ViewMatrix();
	glm::mat4 Matrix();
	rp3d::Vector3 RotationAxis();
	// Handles camera inputs
	void Inputs(GLFWwindow* window);
	void CameraZoom(double offset);
	void CameraHorizontalSlide(double offset);
private:
	char _LastLeftEvent = GLFW_RELEASE;
	char _LastMiddleEvent = GLFW_RELEASE;
	char _LastLSfhitEvent = GLFW_RELEASE;
	char _LastSpaceEvent = GLFW_RELEASE;
	bool _FirstClick = true;
};
#endif