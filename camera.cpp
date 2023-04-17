#include "camera.h"
#include "game_engine.h"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include "math.h"

CameraInfos::CameraInfos(int width, int height, float FOVDeg, glm::vec3 position): Width(width), Height(height), FOVDeg(FOVDeg)
{
	CameraEntity = GameEngine::GetInstance().ECSWorld.entity("camera")
		.set<Transform>({ {position.x, position.y, position.z}, rp3d::Quaternion::fromEulerAngles(0, 0, 0)})
		.set<RigidBody>({ 70.0f, rp3d::BodyType::DYNAMIC, NULL })
		.set<Collider>({ {1, 2, 1}, rp3d::CollisionShapeName::BOX, false, 0, NULL })
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
	auto position = CameraEntity.get<Transform>()->Position;
	auto glmPosition = Position();
	auto view = glm::lookAt(glmPosition, glmPosition + Orientation, Up);
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
	auto translation = Orientation * (float)offset * SensitivityScroll;
	transform->Position += rp3d::Vector3(translation.x, translation.y, translation.z);
}

void CameraInfos::CameraHorizontalSlide(double offset)
{
	// Slide left and right
	auto transform = CameraEntity.get_mut<Transform>();
	auto translation = glm::normalize(glm::cross(Orientation, Up)) * (float)offset * SensitivityScroll;
	transform->Position += rp3d::Vector3(translation.x, translation.y, translation.z);
}

void CameraInfos::Inputs(GLFWwindow* window)
{
	char leftEvent = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	char middleEvent = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
	char lSfhitEvent = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
	char spaceEvent = glfwGetKey(window, GLFW_KEY_SPACE);
	GameEngine& gameEngine = GameEngine::GetInstance();
	auto transform = CameraEntity.get_mut<Transform>();
	double mouseX;
	double mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	// Handles mouse inputs
	// Camera rotation
	if (middleEvent == GLFW_PRESS && lSfhitEvent != GLFW_PRESS)
	{
		// Prevents camera from jumping on the first click
		if (_FirstClick)
		{
			//glfwSetCursorPos(window, ((float)Width / 2), ((float)Height / 2));
			_FirstClick = false;
			_LastMouseX = mouseX;
			_LastMouseY = mouseY;
			
		}

		// And then "transforms" them into degrees 
		float rotX = - SensitivityRotation * (float)(mouseY - _LastMouseY) / Height;
		float rotY = - SensitivityRotation * (float)(mouseX - _LastMouseX) / Width;

		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));


		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrientation;
		}

		// Rotates the Orientation left and right
		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

		_LastMouseX = mouseX;
		_LastMouseY = mouseY;
	}
	else if (middleEvent == GLFW_RELEASE && _LastMiddleEvent == GLFW_PRESS && lSfhitEvent != GLFW_PRESS )
	{
		_FirstClick = true;
	}

	// Camera translation
	if (lSfhitEvent == GLFW_PRESS)
	{
		if (middleEvent == GLFW_PRESS)
		{
			// Stores the coordinates of the cursor
			double mouseX;
			double mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
		if (_FirstClick)
		{
			//glfwSetCursorPos(window, (Width / 2), (Height / 2));
			_FirstClick = false;
			_LastMouseX = mouseX;
			_LastMouseY = mouseY;
		}

		
 
		float translationY = (float)(mouseY - _LastMouseY) / Height;
		float translationX = -(float)(mouseX - _LastMouseX) / Width;

		// Calculates upcoming change in the Position
		glm::vec3 translation = SensitivityTranslation * (glm::normalize(glm::cross(Orientation, Up)) * translationX + glm::normalize(Up) * translationY);

		// Update Position
		transform->Position += rp3d::Vector3(translation.x, translation.y, translation.z);

		_LastMouseX = mouseX;
		_LastMouseY = mouseY;
		}
		else if (middleEvent == GLFW_RELEASE && _LastMiddleEvent == GLFW_PRESS)
		{
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
	_LastLSfhitEvent = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
	_LastSpaceEvent = glfwGetKey(window, GLFW_KEY_SPACE);
}