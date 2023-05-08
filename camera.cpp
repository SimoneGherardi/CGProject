#include "camera.h"
#include "game_engine.h"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include "math.h"
#include "EditorGUI.h"

CameraInfos::CameraInfos(float FOVDeg, glm::vec3 position): FOVDeg(FOVDeg)
{
	SpawnPoint = rp3d::Vector3(position.x, position.y, position.z);
	CameraEntity = GameEngine::GetInstance().ECSWorld.entity("camera")
		.set<Prefab>({PREFABS::PLAYER})
		.set<Transform>({ {SpawnPoint}, rp3d::Quaternion::fromEulerAngles(0, 0, 0)})
		.set<RigidBody>({ 70.0f, rp3d::BodyType::DYNAMIC, false, NULL })
		.set<Collider>({ {3, 3, 3}, rp3d::CollisionShapeName::CAPSULE, false, 0, NULL })
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
	// read inputs
	char leftEvent = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	char middleEvent = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
	char lShiftEvent = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
	char spaceEvent = glfwGetKey(window, GLFW_KEY_SPACE);
	char pEvent = glfwGetKey(window, GLFW_KEY_P);
	GameEngine& gameEngine = GameEngine::GetInstance();
	auto transform = CameraEntity.get_mut<Transform>();
	double mouseX;
	double mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	// Switch between editor and game mode
	if (pEvent == GLFW_RELEASE && _LastPEvent == GLFW_PRESS)
	{
		std::string playingSceneName = "playingScene.txt";
		gameEngine.SetIsEditor(!gameEngine.IsEditor);
		std::cout << spaceEvent << " " << gameEngine.IsEditor << std::endl;
		if (gameEngine.IsEditor)
		{
			auto body = CameraEntity.get<RigidBody>()->Body;
			body->setLinearVelocity(rp3d::Vector3(0,0,0));
			auto transform = CameraEntity.get_mut<Transform>();
			transform->Position = SpawnPoint;
			EditorGUI::GetInstance()->OpenScene(playingSceneName.c_str());
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			_LastMouseX = mouseX;
			_LastMouseY = mouseY;
			EditorGUI::GetInstance()->SaveScene(playingSceneName.c_str());
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	
	if (gameEngine.IsEditor == true)
	{
		// Handles inputs in editor mode
	// Camera rotation
		if (middleEvent == GLFW_PRESS && lShiftEvent != GLFW_PRESS)
		{
			// Prevents camera from jumping on the first click
			if (_FirstClick)
			{
				//glfwSetCursorPos(window, ((float)Width / 2), ((float)Height / 2));
				_FirstClick = false;
				_LastMouseX = mouseX;
				_LastMouseY = mouseY;
			}

			float rotX = -SensitivityRotation * (float)(mouseY - _LastMouseY) / Height;
			float rotY = -SensitivityRotation * (float)(mouseX - _LastMouseX) / Width;

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
		else if (middleEvent == GLFW_RELEASE && _LastMiddleEvent == GLFW_PRESS && lShiftEvent != GLFW_PRESS)
		{
			_FirstClick = true;
		}

		// Camera translation
		if (lShiftEvent == GLFW_PRESS)
		{
			if (middleEvent == GLFW_PRESS)
			{
				glfwGetCursorPos(window, &mouseX, &mouseY);
				if (_FirstClick)
				{
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

		WASDInEditor(window, SpeedEditor);
		
	}
	
	// Handles keyboard inputs for movement in playing mode
	if (gameEngine.IsEditor == false)
	{
		// And then "transforms" them into degrees 
		float rotX = SensitivityRotation * (float)(mouseY - _LastMouseY) / Height;
		float rotY = SensitivityRotation * (float)(mouseX - _LastMouseX) / Width;

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

		if (spaceEvent == GLFW_PRESS && _LastSpaceEvent == GLFW_RELEASE)
		{
			auto body = CameraEntity.get<RigidBody>()->Body;
			auto velocity = body->getLinearVelocity();
			if (velocity.y <= 0.2 && velocity.y >= -0.2)
			{
				velocity.y = 8;
				body->setLinearVelocity(velocity);
			}
		}

		WASDInGame(window, SpeedGame);
	}

	

	_LastLeftEvent = leftEvent;
	_LastMiddleEvent = middleEvent;
	_LastLShiftEvent = lShiftEvent;
	_LastSpaceEvent = spaceEvent;
	_LastPEvent = pEvent;
	
}

void CameraInfos::WASDInEditor(GLFWwindow* window, float speed)
{
	char wEvent = glfwGetKey(window, GLFW_KEY_W);
	char aEvent = glfwGetKey(window, GLFW_KEY_A);
	char sEvent = glfwGetKey(window, GLFW_KEY_S);
	char dEvent = glfwGetKey(window, GLFW_KEY_D);

	glm::vec3 forwardDirection = glm::normalize(glm::vec3(Orientation.x, 0, Orientation.z));
	glm::vec3 rightDirection = glm::normalize(glm::cross(Orientation, Up));
	rightDirection = glm::normalize(glm::vec3(rightDirection.x, rightDirection.y,	rightDirection.z));
	glm::vec3 movementDirection = glm::vec3(0, 0, 0);
	

	if (wEvent == GLFW_PRESS)
	{
		glm::vec3 movement = forwardDirection * (float)GameEngine::GetInstance().DeltaTime.count() * speed;
		movementDirection += movement;
	}
	if (aEvent == GLFW_PRESS)
	{
		glm::vec3 movement = -rightDirection * (float)GameEngine::GetInstance().DeltaTime.count() * speed;
		movementDirection += movement;
	}
	if (sEvent == GLFW_PRESS)
	{
		glm::vec3 movement = -forwardDirection * (float)GameEngine::GetInstance().DeltaTime.count() * speed;
		movementDirection += movement;
	}
	if (dEvent == GLFW_PRESS)
	{
		glm::vec3 movement = rightDirection * (float)GameEngine::GetInstance().DeltaTime.count() * speed;
		movementDirection += movement;
	}
	
	if (movementDirection != glm::vec3(0, 0, 0))
	{
		auto transform = CameraEntity.get_mut<Transform>();
		transform->Position += rp3d::Vector3(movementDirection.x, movementDirection.y, movementDirection.z);
	}

	_LastWEvent = wEvent;
	_LastAEvent = aEvent;
	_LastSEvent = sEvent;
	_LastDEvent = dEvent;
}



void CameraInfos::WASDInGame(GLFWwindow* window, float speed)
{
	char wEvent = glfwGetKey(window, GLFW_KEY_W);
	char aEvent = glfwGetKey(window, GLFW_KEY_A);
	char sEvent = glfwGetKey(window, GLFW_KEY_S);
	char dEvent = glfwGetKey(window, GLFW_KEY_D);

	
	rp3d::Vector3 forwardDirection = rp3d::Vector3(Orientation.x, 0, Orientation.z);
	forwardDirection.normalize();
	auto rightDirectionGlm = glm::normalize(glm::cross(Orientation, Up));
	rp3d::Vector3 rightDirection = rp3d::Vector3(rightDirectionGlm.x, 0, rightDirectionGlm.z);
	rightDirection.normalize();
	rp3d::Vector3 movementDirection	= rp3d::Vector3(0, 0, 0);

	// If the key is pressed, we add the corresponding direction to the movement direction
	if (wEvent == GLFW_PRESS)
	{
		movementDirection += forwardDirection;
	}
	if (aEvent == GLFW_PRESS)
	{
		movementDirection -= rightDirection;
	}
	if (sEvent == GLFW_PRESS)
	{
		movementDirection -= forwardDirection;
	}
	if (dEvent == GLFW_PRESS)
	{
		movementDirection += rightDirection;
	}

	// If the key is released, we remove the corresponding direction to the movement direction
	if (wEvent == GLFW_RELEASE && _LastWEvent == GLFW_PRESS)
	{
		movementDirection -= forwardDirection;
	}
	if (aEvent == GLFW_RELEASE && _LastAEvent == GLFW_PRESS)
	{
		movementDirection += rightDirection;
	}
	if (sEvent == GLFW_RELEASE && _LastSEvent == GLFW_PRESS)
	{
		movementDirection += forwardDirection;
	}
	if (dEvent == GLFW_RELEASE && _LastDEvent == GLFW_PRESS)
	{
		movementDirection -= rightDirection;
	}
	
	auto velocity = CameraEntity.get_mut<Velocity>();
	movementDirection.normalize();
	velocity->Direction = movementDirection;
	velocity->Magnitude = speed;

	
	_LastWEvent = wEvent;
	_LastAEvent = aEvent;
	_LastSEvent = sEvent;
	_LastDEvent = dEvent;
}