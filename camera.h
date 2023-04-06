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

//#include"shaderClass.h"

class CameraInfos
{
public:
	// Stores the main vectors of the camera
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Prevents the camera from jumping around when first clicking left click
	bool firstClick = true;

	float FOVDeg;
	// Stores the width and height of the window
	int Width;
	int Height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float Speed = 0.1f;
	float Sensitivity = 100.0f;

	// Camera constructor to set up initial values
	CameraInfos(int width, int height, float FOVDeg, glm::vec3 position);

	// Updates and exports the camera matrix to the Vertex Shader
	glm::mat4 ProjectionMatrix();
	glm::mat4 ViewMatrix();
	glm::mat4 Matrix();
	// Handles camera inputs
	void Inputs(GLFWwindow* window);
private:
	char _LastLeftEvent = GLFW_RELEASE;
};
#endif