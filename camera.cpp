#include "camera.h"
#include "game_engine.h"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include "math.h"

CameraInfos::CameraInfos(int width, int height, float FOVDeg, glm::vec3 position): Width(width), Height(height), FOVDeg(FOVDeg)
{
	CameraEntity = GameEngine::GetInstance().ECSWorld.entity("camera")
		.set<Transform>({ {position.x, position.y, position.z}, rp3d::Quaternion::fromEulerAngles(0, 0, 0)})
		.set<RigidBody>({ 7.0f, rp3d::BodyType::DYNAMIC, NULL })
		.set<Collider>({ {1, 4, 1}, rp3d::CollisionShapeName::CAPSULE, false, NULL })
		.add<Velocity>();
}

glm::vec3 CameraInfos::Position()
{
	auto transform = CameraEntity.get<Transform>();
	return glm::vec3(transform->Position.x, transform->Position.y, transform->Position.z);
}

glm::mat4 CameraInfos::ProjectionMatrix()
{
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(this->FOVDeg), (float)Width / Height, NEAR_PLANE, FAR_PLANE);
	projection[1][1] *= -1; // Inverts the Y axis
	return projection;
}

glm::mat4 CameraInfos::ViewMatrix()
{
	auto transform = CameraEntity.get<Transform>();
	rp3d::Transform t = rp3d::Transform(rp3d::Vector3(transform->Position.x, transform->Position.y, transform->Position.z), transform->Rotation);
	float matrix[16];
	t.getOpenGLMatrix(matrix);
	auto ModelMatrix = glm::mat4(
		matrix[0], matrix[1], matrix[2], matrix[3],
		matrix[4], matrix[5], matrix[6], matrix[7],
		matrix[8], matrix[9], matrix[10], matrix[11],
		matrix[12], matrix[13], matrix[14], matrix[15]
	);
	return ModelMatrix;
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

rp3d::Vector3 CameraInfos::RotationAxis()
{
	auto transform = CameraEntity.get_mut<Transform>();
	rp3d::Vector3 rotationAxis;
	rp3d::decimal rotationAngle;
	transform->Rotation.getRotationAngleAxis(rotationAngle, rotationAxis);
	return rotationAxis;
}

void CameraInfos::CameraZoom(double offset)
{
	// Zooms in and out
	auto transform = CameraEntity.get_mut<Transform>();
	auto rotationAxis = transform->Rotation.getMatrix() * rp3d::Vector3(0, 0, -1);
	transform->Position += rotationAxis * (float)offset * sensitivityScroll;
}

void CameraInfos::CameraHorizontalSlide(double offset)
{
	// Slide left and right
	auto transform = CameraEntity.get_mut<Transform>();
	auto rotationAxis = RotationAxis();
	rp3d::Vector3 xAxis = transform->Rotation.getMatrix() * rp3d::Vector3(1, 0, 0);
	transform->Position += xAxis * (float)offset * sensitivityScroll;
}

void CameraInfos::Inputs(GLFWwindow* window)
{
	char leftEvent = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	char middleEvent = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
	char lSfhitEvent = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
	char spaceEvent = glfwGetKey(window, GLFW_KEY_SPACE);
	GameEngine& gameEngine = GameEngine::GetInstance();
	auto transform = CameraEntity.get_mut<Transform>();

	int lastKey = GLFW_KEY_LAST;
	// Handles mouse inputs
	// Camera rotation
	if (middleEvent == GLFW_PRESS && lSfhitEvent != GLFW_PRESS)
	{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		
		// Prevents camera from jumping on the first click
		if (_FirstClick)
		{
			glfwSetCursorPos(window, ((float)Width / 2), ((float)Height / 2));
			_FirstClick = false;
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
		auto newRotation = transform->Rotation * rp3d::Quaternion::fromEulerAngles(rotX, rotY, 0);
		newRotation.normalize();

		// Decides whether or not the next vertical Orientation is legal or not
		/*if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrientation;
		}*/

		// Rotates the Orientation left and right
		transform->Rotation = newRotation;

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, ((float)Width / 2), ((float)Height / 2));
	}
	else if (middleEvent == GLFW_RELEASE && _LastMiddleEvent == GLFW_PRESS && lSfhitEvent != GLFW_PRESS )
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		
		// Makes sure the next time the camera looks around it doesn't jump
		_FirstClick = true;
	}

	// Camera translation
	if (lSfhitEvent == GLFW_PRESS)
	{
		if (middleEvent == GLFW_PRESS)
		{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		
		// Prevents camera from jumping on the first click
		if (_FirstClick)
		{
			glfwSetCursorPos(window, (Width / 2), (Height / 2));
			_FirstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);
 
		float translationY = (float)(mouseY - (Height / 2)) / Height;
		float translationX = -(float)(mouseX - (Width / 2)) / Width;

		// Calculates upcoming change in the Position
		auto xAxis = transform->Rotation.getMatrix() * rp3d::Vector3(1, 0, 0);
		auto yAxis = transform->Rotation.getMatrix() * rp3d::Vector3(0, 1, 0);
		rp3d::Vector3 translation = xAxis * translationX + yAxis * translationY;

		// Update Position
		transform->Position += sensitivityTranslation * translation;

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (Width / 2), (Height / 2));
		}
		else if (middleEvent == GLFW_RELEASE && _LastMiddleEvent == GLFW_PRESS)
		{
			// Unhides cursor since camera is not looking around anymore
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			
			// Makes sure the next time the camera looks around it doesn't jump
			_FirstClick = true;
		}
	}

	
	if (spaceEvent == GLFW_RELEASE && _LastSpaceEvent == GLFW_PRESS)
	{
		gameEngine.SetIsEditor(!gameEngine.IsEditor);
		std::cout << spaceEvent << " " << gameEngine.IsEditor << std::endl;
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

	_LastLeftEvent = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	_LastMiddleEvent = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
	_LastLSfhitEvent = glfwGetMouseButton(window, GLFW_KEY_LEFT_SHIFT);
	_LastSpaceEvent = glfwGetKey(window, GLFW_KEY_SPACE);
}