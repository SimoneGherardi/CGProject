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

double global_yoffset = 0;
double global_xoffset = 0;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	global_yoffset = yoffset;
	global_xoffset = xoffset;
}

void CameraInfos::CameraZoom(double offset)
{
	// Zooms in and out
	Position += Orientation * (float)offset * sensitivityScroll;
}

void CameraInfos::CameraHorizontalSlide(double offset)
{
	// Slide left and right
	glm::vec3 Horizontal = glm::normalize(glm::cross(Orientation, Up));
	Position += Horizontal * (float)offset * sensitivityScroll;
}



void CameraInfos::ScaledGetCursorPos(GLFWwindow* window, double* xpos, double* ypos, float windowScaleFactor, WindowSize windowSize, float horizontalOffset, float verticalOffset) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
	horizontalOffset = verticalOffset / 3.5;
    if (mouseX > horizontalOffset && mouseX <= windowSize.Width*windowScaleFactor + horizontalOffset && mouseY > verticalOffset && mouseY < windowSize.Width * windowScaleFactor + 1.5 * verticalOffset) {
		*xpos = (mouseX - horizontalOffset) / windowScaleFactor;
		*ypos = (mouseY - 1.5 * verticalOffset) / windowScaleFactor;
	}
    else {
		*xpos = mouseX;
		*ypos = mouseY;
	}
}

void CameraInfos::Inputs(GLFWwindow* window, float windowScaleFactor, WindowSize windowSize, float horizontalOffset, float verticalOffset)
{

	// Handles mouse inputs
	// Camera rotation
	float ScaledWidth = Width * windowScaleFactor + horizontalOffset;
	float ScaledHeight = Height * windowScaleFactor + verticalOffset;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
	{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		
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
		float rotX = - sensitivityRotation * (float)(mouseY - (Height / 2)) / Height;
		float rotY = - sensitivityRotation * (float)(mouseX - (Width / 2)) / Width;

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
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		
		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, (Width / 2), (Height / 2));
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);
 
		float translationY = (float)(mouseY - (Height / 2)) / Height;
		float translationX = -(float)(mouseX - (Width / 2)) / Width;

		// Calculates upcoming change in the Position
		glm::vec3 translation = glm::normalize(glm::cross(Orientation, Up)) * translationX + glm::normalize(Up) * translationY;

		// Update Position
		Position += sensitivityTranslation * translation;

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (Width / 2), (Height / 2));
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

	// Mouse left button
	if (_LastLeftEvent == GLFW_PRESS && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		ScaledGetCursorPos(window, &mouseX, &mouseY, windowScaleFactor, windowSize, horizontalOffset, verticalOffset);

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