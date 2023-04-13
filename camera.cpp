#include "camera.h"
#include "game_engine.h"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"

CameraInfos::CameraInfos(int width, int height, float FOVDeg, glm::vec3 position): Width(width), Height(height), FOVDeg(FOVDeg), Position(position)
{}

glm::mat4 CameraInfos::ProjectionMatrix()
{
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(this->FOVDeg), (float)Width / Height, NEAR_PLANE, FAR_PLANE);
	projection[1][1] *= -1; // Inverts the Y axis
	return projection;
}

glm::mat4 CameraInfos::ViewMatrix()
{
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(Position, Position + Orientation, Up);
	return view;
}

glm::mat4 CameraInfos::Matrix()
{
	glm::mat4 projection = ProjectionMatrix();
	glm::mat4 view = ViewMatrix();

	return projection * view;
}

void CameraInfos::Inputs(GLFWwindow* window)
{
	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, ((float)Width / 2), ((float)Height / 2));
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = Sensitivity * (float)(mouseY - ((float)Height / 2)) / Height;
		float rotY = Sensitivity * (float)(mouseX - ((float)Width / 2)) / Width;

		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrientation;
		}

		// Rotates the Orientation left and right
		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, ((float)Width / 2), ((float)Height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	}

	if (_LastLeftEvent == GLFW_PRESS && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);
		auto mousePosition = glm::vec2((mouseX / Width * 2) - 1, (mouseY / Height) * 2 - 1);
		std::cout << "mousePosition: " << glm::to_string(mousePosition) << std::endl;
		std::vector<RaycastInfo*> raycasts = GameEngine::GetInstance().RaycastFromCamera(mousePosition, 10);

		printf("Raycast results: %d\n", raycasts.size());

		for (RaycastInfo* raycast : raycasts)
		{
			std::cout << raycast->worldPoint.to_string();
			std::cout << raycast->Entity.name() << std::endl;
		}
	}

	_LastLeftEvent = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
}