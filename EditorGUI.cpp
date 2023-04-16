#include "EditorGUI.h"
#include "game_engine.h"
#include "reactphysics3d/reactphysics3d.h"
#include "ecs_modules.h"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <glm/ext/matrix_transform.hpp>
#include "Models.h"
#include "Angles.h"

EditorGUI* EditorGUI::_Instance = nullptr;

EditorGUI::EditorGUI() {};

EditorGUI* EditorGUI::GetInstance() {
    if (_Instance == nullptr) {
        _Instance = new EditorGUI();
    }
    return _Instance;
}

void EditorGUI::Initialize(WindowSize windowSize, GLFWwindow* window){
    _Window = window;
    WindowHeight = windowSize.Height;
    WindowWidth = windowSize.Width;
    ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ShowDemoWindow = false;
    ShowAnotherWindow = true;

    // Dimensions
    ScaleFactor = 0.8f;
    MenuBarHeight = ((float)windowSize.Height) * 0.04;
    HorizontalBorder = 0.0f;
    MenuBarDimensions = ImVec2(((float)windowSize.Width), MenuBarHeight);
    SceneDimensions = ImVec2(((float)windowSize.Width) * ScaleFactor, ((float)windowSize.Height) * ScaleFactor + 0.5 * MenuBarHeight);
    PrefabContainerDimensions = ImVec2(SceneDimensions[0], ((float)windowSize.Height) - SceneDimensions.y);
    LogDimensions = ImVec2(((float)windowSize.Width) * (1.0f - ScaleFactor), ((float)windowSize.Height) - MenuBarHeight);
    LogEditPromptDimensions = ImVec2(LogDimensions.x, LogDimensions.y * 0.5);

    // Positions
    SceneCenterPosition = glm::vec2((((SceneDimensions[0] / 2) + HorizontalBorder) / WindowWidth * 2) - 1, (((SceneDimensions[1] / 2) + MenuBarHeight) / WindowHeight) * 2 - 1);
    MenuBarPosition = ImVec2(0, 0);
    ScenePosition = ImVec2(0, MenuBarHeight);
    PrefabContainerPosition = ImVec2(0, SceneDimensions.y + MenuBarHeight);
    LogPosition = ImVec2(SceneDimensions.x, MenuBarHeight);
    LogEditPromptPositions = ImVec2(LogPosition.x, LogDimensions.y * 2/3);
}

bool EditorGUI::CheckMouseInsideScene(float mouseX, float mouseY) {
    if (mouseX > ScenePosition.x && mouseX <= LogPosition.x && mouseY > ScenePosition.y && mouseY < PrefabContainerPosition.y) {
        return true;
    }
    else {
        return false;
    }
}

bool EditorGUI::CheckMouseInsidePrompt(float mouseX, float mouseY) {
    if (mouseX > LogEditPromptPositions.x && mouseX <= LogEditPromptPositions.x + LogEditPromptDimensions.x && mouseY > LogEditPromptPositions.y && mouseY < LogEditPromptPositions.y + LogEntryDimensions.y) {
        return true;
    }
    else {
        return false;
    }
}

double EditorGUI::ScaleMouseX(double mouseX) {
	return (mouseX - HorizontalBorder) / ScaleFactor;
}

double EditorGUI::ScaleMouseY(double mouseY) {
    return (mouseY - 1.5 * MenuBarHeight) / ScaleFactor;
}

bool EditorGUI::ScaledGetCursorPos(GLFWwindow* window, double* xpos, double* ypos) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    EditorGUI* editorGUI = EditorGUI::GetInstance();
    HorizontalBorder = MenuBarHeight / 3.5;
    if (editorGUI->CheckMouseInsideScene(mouseX, mouseY)) {
        *xpos = ScaleMouseX(mouseX);
        *ypos = ScaleMouseY(mouseY);
        return true;
    }
    else {
        *xpos = mouseX;
        *ypos = mouseY;
        return false;
    }
}

double EditorGUI::MouseToNorm(double mouse, double dimension) {
    // Converts mouse coordinates to normalized coordinates, dimension is the width or height of the window for respectively mouse x or y
    return ((mouse / dimension) * 2) - 1;
}

double EditorGUI::NormToMouse(double norm, double dimension) {
    // Converts normalized coordinates to mouse coordinates, dimension is the width or height of the window for respectively mouse x or y
    return ((-norm + 1) / 2) * dimension;
}



void EditorGUI::Inputs(GLFWwindow* window) {
    // Mouse left button
    double mouseX;
    double mouseY;
    double mouseXSpawn;
    double mouseYSpawn;

    char leftEvent = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    GameEngine& gameEngine = GameEngine::GetInstance();

    if (ScaledGetCursorPos(window, &mouseX, &mouseY))
    {
        // Only inside Scene
        // Left mouse button pressed
        if (leftEvent == GLFW_PRESS)
        {
            
            // Prevents camera from jumping on the first click
            if (_FirstClick)
            {
                glm::vec2 mousePositionNorm = glm::vec2(MouseToNorm(mouseX, WindowWidth), MouseToNorm(mouseY, WindowHeight));
                std::vector<RaycastInfo*> raycasts = gameEngine.RaycastFromCamera(mousePositionNorm, 10);
                if (raycasts.size() > 0) {
                    GameEngine::GetInstance().SelectedEntityId = raycasts[0]->Entity;
                }
                else {
                    GameEngine::GetInstance().SelectedEntityId = FLECS_INVALID_ENTITY;
                }
                _FirstClick = false;

                //set Last mouse
                _LastMouseX = mouseX;
                _LastMouseY = mouseY;
            }
            if (GameEngine::GetInstance().SelectedEntityId != FLECS_INVALID_ENTITY) {

                // Fetches the coordinates of the cursor
                ScaledGetCursorPos(window, &mouseX, &mouseY);
                glm::vec2 mousePositionNorm = glm::vec2(MouseToNorm(mouseX, WindowWidth), MouseToNorm(mouseY, WindowHeight));
                glm::vec2 lastmousePositionNorm = glm::vec2(MouseToNorm(_LastMouseX, WindowWidth), MouseToNorm(_LastMouseY, WindowHeight));

                rp3d::Vector3 cameraPosition = gameEngine.WorldToCameraSpace(gameEngine.SelectedEntity().get<Transform>()->Position);

                cameraPosition.x += (mousePositionNorm.x - lastmousePositionNorm.x)/ScaleFactor;
                cameraPosition.y -= (mousePositionNorm.y - lastmousePositionNorm.y)/ScaleFactor;


                rp3d::Vector3 newWorldPosition = gameEngine.CameraToWorldSpace(cameraPosition);

                Transform* transform = gameEngine.SelectedEntity().get_mut<Transform>();
                transform->Position.x = newWorldPosition.x;
                transform->Position.y = newWorldPosition.y;
                transform->Position.z = newWorldPosition.z;

                _LastMouseX = mouseX;
                _LastMouseY = mouseY;
            }

        }

        // Left mouse button released
        if (_LastLeftEvent == GLFW_PRESS && leftEvent == GLFW_RELEASE)
        {
            auto mousePosition = glm::vec2(MouseToNorm(mouseX, WindowWidth), MouseToNorm(mouseY, WindowHeight));
            std::cout << "mousePosition: " << glm::to_string(mousePosition) << std::endl;

            std::vector<RaycastInfo*> raycasts = gameEngine.RaycastFromCamera(mousePosition, 10);

            if (raycasts.size() > 0) {
                gameEngine.SelectedEntityId = raycasts[0]->Entity;
            }
            else {
                gameEngine.SelectedEntityId = FLECS_INVALID_ENTITY;
            }


            //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            
            _FirstClick = true;
        }
    
    }
    
    _LastLeftEvent = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
}

void EditorGUI::CheckSpaceForPrompt(double* spawnX, double* spawnY, ImVec2 dimensions) {
    // check if there is space for the prompt, otherwise adjusts the spwn coordinates
    if (*spawnX + dimensions.x > WindowWidth) {
		*spawnX -= dimensions.x;
	}
    if (*spawnY + dimensions.y > WindowHeight) {
		*spawnY -= dimensions.y;
	}

}

void EditorGUI::PrintPrompt() {
    GameEngine& gameEngine = GameEngine::GetInstance();
    if (gameEngine.SelectedEntityId == 0) {
		return;
	}
    ImGui::SetNextWindowSize(LogEditPromptDimensions);
    ImGui::SetNextWindowPos(LogEditPromptPositions);

    rp3d::Vector3 tmpPosition;
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
    ImGui::Begin("Prompt", NULL, flags);
    std::string PromptText = gameEngine.SelectedEntity().name().c_str() + std::string(" info");
    ImGui::Text(PromptText.c_str());
    
    ImGui::NewLine();
    // Managing rotation
    
    std::string ButtonName = "Position";
    
    // Managing position
    tmpPosition = gameEngine.SelectedEntity().get<Transform>()->Position;
    float tmpPositionVec[3] = { tmpPosition.x, tmpPosition.y, tmpPosition.z };
    ImGui::InputFloat3(ButtonName.c_str(), tmpPositionVec);
    tmpPosition = rp3d::Vector3(tmpPositionVec[0], tmpPositionVec[1], tmpPositionVec[2]);

    ButtonName = "Rotation";
    AnglesQuaternion tmpQuat = AnglesQuaternion(gameEngine.SelectedEntity().get<Transform>()->Rotation);
    AnglesEulerAngles tmpRotation = ToAnglesEulerAngles(tmpQuat);
    tmpRotation.toDegrees();
    ImGui::InputFloat3(ButtonName.c_str(), tmpRotation.XYZ);
    tmpRotation.toRadians();
    rp3d::Quaternion newRotation = rp3d::Quaternion::fromEulerAngles(tmpRotation.XYZ[0], tmpRotation.XYZ[1], tmpRotation.XYZ[2]);
    
    gameEngine.SelectedEntity().set<Transform>({ tmpPosition, newRotation });
    ImGui::End();
}

void EditorGUI::ShowCustomWindow(ImTextureID renderTexture, WindowSize windowSize, GLFWwindow* window) {

    // Menu bar
    ImGui::SetNextWindowSize(MenuBarDimensions);
    ImGui::SetNextWindowPos(MenuBarPosition);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
    ImGui::Begin("Menu Bar", NULL, flags);
    if (ImGui::Button("Open")) {
        Counter++;
    }
    ImGui::SameLine();
    if (ImGui::Button("Save")) {}
    ImGui::End();

    // Scene 
    ImGui::SetNextWindowSize(SceneDimensions);
    ImGui::SetNextWindowPos(ScenePosition);
    flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("Scene", NULL, flags);                          // Create a window called "Hello, world!" and append into it.
    ImGui::Image(renderTexture, ImVec2(windowSize.Width * ScaleFactor, windowSize.Height * ScaleFactor));
    ImGuiStyle& style = ImGui::GetStyle();
    HorizontalBorder = style.WindowBorderSize;
    ImGui::End();
    ImGui::PopStyleVar();


    //PrefabContainer
    ImGui::SetNextWindowSize(PrefabContainerDimensions);
    ImGui::SetNextWindowPos(PrefabContainerPosition);
    flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;


    ImGui::Begin("Prefab Container", NULL, flags);
    if (ImGui::Button("Square Block", ButtonDimensions)) {
        std::string name = "SUZA";
        name += std::to_string(GameEngine::GetInstance().Entities.size());
        GameEngine::GetInstance().InstantiateEntity(PREFABS::MONKEY, name.c_str())
            .set<Transform>({ GameEngine::GetInstance().ScreenToWorldSpace(glm::vec3(0, 0, 0.95)) });
    }
    ImGui::SameLine();
    if (ImGui::Button("T Block", ButtonDimensions)) {
    }
    ImGui::SameLine();
    if (ImGui::Button("L Block", ButtonDimensions)) {
    }
    ImGui::SameLine();
    if (ImGui::Button("Reverse L Block", ButtonDimensions)) {
    }
    ImGui::SameLine();
    if (ImGui::Button("Z Block", ButtonDimensions)) {
    }
    ImGui::SameLine();
    if (ImGui::Button("Reverse Z Block", ButtonDimensions)) {
    }
    ImGui::SameLine();

    ImGui::End();

    //Log
    ImGui::SetNextWindowSize(LogDimensions);
    ImGui::SetNextWindowPos(LogPosition);
    flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("Log", NULL, flags);    
    ImGui::PushItemWidth(LogDimensions.x - MenuBarHeight*0.4);
    uint32_t index = 0;
    GameEngine& gameEngine = GameEngine::GetInstance();
    ImGui::ListBoxHeader(" ", gameEngine.Entities.size(), gameEngine.Entities.size());
        for (auto& entity : gameEngine.Entities)
        {
            std::string tmp = "##" + std::to_string(entity.id());
            //ImGui::SameLine();
            if (ImGui::Selectable(tmp.c_str(), gameEngine.SelectedEntityId==entity))
            {
                gameEngine.SelectedEntityId = entity;
            }
            ImGui::SameLine();
            ImGui::Text(entity.name());
        }
    ImGui::ListBoxFooter();
    ImGui::End();

    PrintPrompt();
}

