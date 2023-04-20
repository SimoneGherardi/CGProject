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
#include <chrono>
#include "flecs.h"
#include "reactphysics3d/reactphysics3d.h"

class CameraInfos
{
public:
	// Stores the main vectors of the camera
	float FOVDeg;
	// Stores the width and height of the window
	int Width = 1;
	int Height = 1;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float SpeedGame = 10.0f;
	float SpeedEditor = 15.0f;
	float SensitivityRotation = 100.0f;
	float SensitivityTranslation = 15.0f;
	float SensitivityInGameTranslation = 10.0f;
	float SensitivityScroll = 0.5f;

	// Camera constructor to set up initial values
	CameraInfos(float FOVDeg, glm::vec3 position);

	glm::vec3 Orientation = glm::normalize(glm::vec3(0, -0.4, -1));
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	flecs::entity CameraEntity;
	rp3d::Vector3 SpawnPoint;
	glm::vec3 Position();
	glm::mat4 ProjectionMatrix();
	glm::mat4 ViewMatrix();
	glm::mat4 Matrix();
	rp3d::Vector3 RotationAxis();
	// Handles camera inputs
	void Inputs(GLFWwindow* window);
	void CameraZoom(double offset);
	void CameraHorizontalSlide(double offset);
	void WASDInEditor(GLFWwindow* window, float speed);
	void WASDInGame(GLFWwindow* window, float speed);
private:
	double _LastMouseX;
	double _LastMouseY;
	char _LastLeftEvent = GLFW_RELEASE;
	char _LastMiddleEvent = GLFW_RELEASE;
	char _LastLShiftEvent = GLFW_RELEASE;
	char _LastPEvent = GLFW_RELEASE;
	char _LastWEvent = GLFW_RELEASE;
	char _LastAEvent = GLFW_RELEASE;
	char _LastSEvent = GLFW_RELEASE;
	char _LastDEvent = GLFW_RELEASE;
	char _LastSpaceEvent = GLFW_RELEASE;
	bool _FirstClick = true;
	double _JumpDuration;
	std::chrono::time_point<std::chrono::system_clock> _LastWTime;
	std::chrono::time_point<std::chrono::system_clock> _LastATime;
	std::chrono::time_point<std::chrono::system_clock> _LastSTime;
	std::chrono::time_point<std::chrono::system_clock> _LastDTime;
};
#endif