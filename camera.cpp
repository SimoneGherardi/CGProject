#include "camera.h"

double global_yoffset = 0;
double global_xoffset = 0;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	global_yoffset = yoffset;
	global_xoffset = xoffset;
}

void CameraTest::CameraZoom(double offset)
{
	// Zooms in and out
	Position += Orientation * (float)offset * sensitivityScroll;
}

void CameraTest::CameraHorizontalSlide(double offset)
{
	// Slide left and right
	glm::vec3 Horizontal = glm::normalize(glm::cross(Orientation, Up));
	Position += Horizontal * (float)offset * sensitivityScroll;
}

CameraTest::CameraTest(int width, int height, glm::vec3 position)
{
	CameraTest::width = width;
	CameraTest::height = height;
	Position = position;
}


glm::mat4 CameraTest::Matrix(float FOVdeg, float nearPlane, float farPlane)
{
	// Initializes matrices since otherwise they will be the null matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Makes camera look in the right direction from the right position
	view = glm::lookAt(Position, Position + Orientation, Up);
	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);
	projection[1][1] *= -1; // Inverts the Y axis

	return projection * view;
}


void CameraTest::Inputs(GLFWwindow* window)
{
	// Handles mouse inputs
	// Camera rotation
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
	{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = sensitivityRotation * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivityRotation * (float)(mouseX - (width / 2)) / width;

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
		glfwSetCursorPos(window, (width / 2), (height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	}

	// Camera translation
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
		{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);
 
		float translationY = - sensitivityTranslation * (float)(mouseY - (height / 2)) / height;
		float translationX = sensitivityTranslation * (float)(mouseX - (width / 2)) / width;

		// Calculates upcoming change in the Position
		glm::vec3 translation = glm::normalize(glm::cross(Orientation, Up)) * translationX + glm::normalize(Orientation) * translationY;

		// Update Position
		Position += translation;

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (width / 2), (height / 2));
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE)
		{
			// Unhides cursor since camera is not looking around anymore
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			// Makes sure the next time the camera looks around it doesn't jump
			firstClick = true;
		}
	}
	

	// Handles scroll and swipe inputs
	double yoffset = 0;
	double xoffset = 0;
	glfwSetScrollCallback(window, scroll_callback);
	yoffset = global_yoffset;
	xoffset = global_xoffset;
	if (yoffset != 0)
	{
		CameraZoom(yoffset);
		global_yoffset = 0;
	};
	if (xoffset != 0)
	{
		CameraHorizontalSlide(xoffset);
		global_xoffset = 0;
	};
}