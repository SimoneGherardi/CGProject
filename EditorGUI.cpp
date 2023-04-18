#include "EditorGUI.h"
#include "reactphysics3d/reactphysics3d.h"
#include "ecs_modules.h"
#include "glm/gtx/string_cast.hpp"
#include <glm/ext/matrix_transform.hpp>
#include "Models.h"
#include "Angles.h"
#include "prefab.h"

EditorGUI* EditorGUI::_Instance = nullptr;

EditorGUI::EditorGUI() {};

EditorGUI* EditorGUI::GetInstance() {
    if (_Instance == nullptr) {
        _Instance = new EditorGUI();
    }
    return _Instance;
}

void EditorGUI::SetPrefabsMap() {
    Prefabs.insert({ PREFABS::MONKEY , "Monkey" });
    Prefabs.insert({ PREFABS::BUSH , "Bush"});
    Prefabs.insert({ PREFABS::COIN , "Coin"});
    Prefabs.insert({ PREFABS::GRASSBLOCK , "Grass Block"});
    Prefabs.insert({ PREFABS::ROCK1 , "Rock 1"});
    Prefabs.insert({ PREFABS::ROCK2 , "Rock 2"});
    Prefabs.insert({ PREFABS::SIGN , "Sign"});
    Prefabs.insert({ PREFABS::TREE1 , "Tree 1"});
    Prefabs.insert({ PREFABS::TREE2 , "Tree 2"});
    Prefabs.insert({ PREFABS::WOODBRIDGE , "Wood Bridge"});
    Prefabs.insert({ PREFABS::WOODPLATFORM , "Wood Platform"});
    Prefabs.insert({ PREFABS::WOODSHELF , "Wood Shelf"});
    Prefabs.insert({ PREFABS::CUBE , "Cube"});
    Prefabs.insert({ PREFABS::BRICK , "Brick" });
}

void EditorGUI::Initialize(WindowSize windowSize, GLFWwindow* window){
    _Window = window;
    WindowHeight = windowSize.Height;
    WindowWidth = windowSize.Width;
    ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ShowDemoWindow = false;
    ShowAnotherWindow = true;
    _OpenPrompt = false;
    _SavePrompt = false;

    // Dimensions
    ScaleFactor = 0.7f;
    MenuBarHeight = ((float)windowSize.Height) * 0.04;
    HorizontalBorder = 0.0f;
    MenuBarDimensions = ImVec2(((float)windowSize.Width), MenuBarHeight);
    SceneDimensions = ImVec2(((float)windowSize.Width) * ScaleFactor, ((float)windowSize.Height) * ScaleFactor + 0.5 * MenuBarHeight);
    PrefabContainerDimensions = ImVec2(SceneDimensions[0], ((float)windowSize.Height) - SceneDimensions.y - MenuBarHeight);
    LogDimensions = ImVec2(((float)windowSize.Width) * (1.0f - ScaleFactor), ((float)windowSize.Height) - MenuBarHeight);
    LogEditPromptDimensions = ImVec2(LogDimensions.x, LogDimensions.y * 0.5);
    ButtonDimensions = ImVec2((float)windowSize.Width * 0.09, (float)windowSize.Width * 0.05);
    LogEntryDimensions = ImVec2(LogDimensions.y, ((float)windowSize.Height) * 0.1);
    OpenSavePromptDimensions = ImVec2((float)windowSize.Width * 0.3, (float)windowSize.Width * 0.05);

    // Positions
    SceneCenterPosition = glm::vec2((((SceneDimensions[0] / 2) + HorizontalBorder) / WindowWidth * 2) - 1, (((SceneDimensions[1] / 2) + MenuBarHeight) / WindowHeight) * 2 - 1);
    MenuBarPosition = ImVec2(0, 0);
    ScenePosition = ImVec2(0, MenuBarHeight);
    PrefabContainerPosition = ImVec2(0, SceneDimensions.y + MenuBarHeight);
    LogPosition = ImVec2(SceneDimensions.x, MenuBarHeight);
    LogEditPromptPosition = ImVec2(LogPosition.x, LogDimensions.y * 2/3);
    OpenSavePromptPosition = ImVec2((WindowWidth - OpenSavePromptDimensions.x) / 2, (WindowHeight - OpenSavePromptDimensions.y) / 2);
    
    SetPrefabsMap();
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
    if (mouseX > LogEditPromptPosition.x && mouseX <= LogEditPromptPosition.x + LogEditPromptDimensions.x && mouseY > LogEditPromptPosition.y && mouseY < LogEditPromptPosition.y + LogEntryDimensions.y) {
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

    if (_OpenPrompt || _SavePrompt) {
		return;
	}
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
                std::vector<RaycastInfo*> raycasts = gameEngine.RaycastFromCamera(mousePositionNorm, 100);
                if (raycasts.size() > 0) {
                    GameEngine::GetInstance().SelectedEntityId = raycasts[0]->Entity;
                    rp3d::Vector3 cameraPosition = gameEngine.WorldToCameraSpace(gameEngine.SelectedEntity().get<Transform>()->Position);
                    _FirstZValueObject = cameraPosition.z;
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
                
                glm::vec2 mousePositionNorm = glm::vec2(MouseToNorm(mouseX, WindowWidth), MouseToNorm(mouseY, WindowHeight));
                glm::vec2 lastMousePositionNorm = glm::vec2(MouseToNorm(_LastMouseX, WindowWidth), MouseToNorm(_LastMouseY, WindowHeight));

                rp3d::Vector3 cameraPosition = gameEngine.WorldToCameraSpace(gameEngine.SelectedEntity().get<Transform>()->Position);

                cameraPosition.x += (mousePositionNorm.x - lastMousePositionNorm.x) * (-_FirstZValueObject);
                cameraPosition.y -= (mousePositionNorm.y - lastMousePositionNorm.y) * (-_FirstZValueObject) * 0.7;
                cameraPosition.z = _FirstZValueObject;

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
    ImGui::SetNextWindowPos(LogEditPromptPosition);

    rp3d::Vector3 tmpPosition;
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
    ImGui::Begin("Prompt", NULL, flags);
    std::string PromptText = gameEngine.SelectedEntity().name().c_str() + std::string(" info");
    ImGui::Text(PromptText.c_str());
    
    ImGui::NewLine();
    // Managing rotation
    
    std::string Label = "Position";
    
    // Managing position
    tmpPosition = gameEngine.SelectedEntity().get<Transform>()->Position;
    float tmpPositionVec[3] = { tmpPosition.x, tmpPosition.y, tmpPosition.z };
    ImGui::InputFloat3(Label.c_str(), tmpPositionVec);
    tmpPosition = rp3d::Vector3(tmpPositionVec[0], tmpPositionVec[1], tmpPositionVec[2]);

    Label = "Rotation";
    AnglesQuaternion tmpQuat = AnglesQuaternion(gameEngine.SelectedEntity().get<Transform>()->Rotation);
    AnglesEulerAngles tmpRotation = ToAnglesEulerAngles(tmpQuat);
    tmpRotation.toDegrees();
    ImGui::InputFloat3(Label.c_str(), tmpRotation.XYZ);
    tmpRotation.toRadians();
    rp3d::Quaternion newRotation = rp3d::Quaternion::fromEulerAngles(tmpRotation.XYZ[0], tmpRotation.XYZ[1], tmpRotation.XYZ[2]);
    
    gameEngine.SelectedEntity().set<Transform>({ tmpPosition, newRotation });

    std::string ButtonName = "Delete Object";
    if (ImGui::Button(ButtonName.c_str(), ButtonDimensions)) {
		if (gameEngine.SelectedEntityId != 0) 
        {
            gameEngine.DeleteEntity(gameEngine.SelectedEntity());
            gameEngine.SelectedEntityId = 0;
        }
	}
    ImGui::End();
}

void EditorGUI::PrefabAddButton(const char* label, PREFABS prefab) {
    if (ImGui::Button(label, ButtonDimensions)) {
        std::string name = label;
        name = std::to_string(GameEngine::GetInstance().Entities.size()) + " " + name;
        GameEngine::GetInstance().SelectedEntityId = GameEngine::GetInstance().InstantiateEntity(prefab, name.c_str())
            .set<Transform>({ GameEngine::GetInstance().ScreenToWorldSpace(glm::vec3(0, 0, 0.95)) });
    }
}

void EditorGUI::ShowCustomWindow(ImTextureID renderTexture, WindowSize windowSize, GLFWwindow* window) {

    // Menu bar
    ImGui::SetNextWindowSize(MenuBarDimensions);
    ImGui::SetNextWindowPos(MenuBarPosition);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
    ImGui::Begin("Menu Bar", NULL, flags);
    if (ImGui::Button("Open") || _OpenPrompt) 
    {
        GameEngine::GetInstance().DeserializeEntities("scene.txt");
        /*
        _OpenPrompt = true;
        char* buff = (char*)calloc(128, 1);
        ImGui::SetNextWindowSize(OpenSavePromptDimensions);
        ImGui::SetNextWindowPos(OpenSavePromptPosition);
        ImGui::Begin("Open", NULL, flags);
        ImGui::InputTextWithHint("##fileopen", "Insert file name", buff, 256);
        if (ImGui::Button("Open"))
        {
			GameEngine::GetInstance().DeserializeEntities(buff);
            _OpenPrompt = false;
		}
        
        ImGui::End();
        */
    }
    ImGui::SameLine();
    if (ImGui::Button("Save") || _SavePrompt) 
    {
        GameEngine::GetInstance().SerializeEntities("scene.txt");
        /*
        _SavePrompt = true;
        char* buff = (char*)calloc(128, 1);
		ImGui::SetNextWindowSize(OpenSavePromptDimensions);
		ImGui::SetNextWindowPos(OpenSavePromptPosition);
		ImGui::Begin("Save", NULL, flags);
        ImGui::InputTextWithHint("##filesave", "Insert file name", buff, 256);
        if (ImGui::Button("Save"))
        {
			GameEngine::GetInstance().SerializeEntities(buff);
            _SavePrompt = false;
		}
        
		ImGui::End();
        */
    }

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
    float prefabWindowSize = PrefabContainerDimensions.x;
    for (auto const& [key, val] : Prefabs)
    {
        PrefabAddButton(val.c_str(), key);
        prefabWindowSize -= ButtonDimensions.x;
        if (prefabWindowSize > ButtonDimensions.x)
        {
            ImGui::SameLine();
        }
        else
        {
            prefabWindowSize = PrefabContainerDimensions.x;
        }
        
    }

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

