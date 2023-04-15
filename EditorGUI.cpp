#include "EditorGUI.h"
#include "game_engine.h"
#include "reactphysics3d/reactphysics3d.h"
#include "ecs_modules.h"
#include "Models.h"

EditorGUI* EditorGUI::_Instance = nullptr;

EditorGUI::EditorGUI() {};

LogEntry::LogEntry(flecs::entity* entity, bool isSelected): Entity(entity), IsSelected(isSelected) {
    std::cout << "LogEntry created" << std::endl;
}

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
    LogEditPromptPositions = ImVec2(LogPosition.x, WindowHeight * 2/3);
}

bool EditorGUI::CheckMouseInsideScene(float mouseX, float mouseY) {
    if (mouseX > ScenePosition.x && mouseX <= LogPosition.x && mouseY > ScenePosition.y && mouseY < PrefabContainerPosition.y) {
        return true;
    }
    else {
        return false;
    }
}

void EditorGUI::AddLogEntry(flecs::entity* entity) {
    Log.push_back(LogEntry(entity, false));
}

bool EditorGUI::ScaledGetCursorPos(GLFWwindow* window, double* xpos, double* ypos) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    EditorGUI* editorGUI = EditorGUI::GetInstance();
    HorizontalBorder = MenuBarHeight / 3.5;
    if (editorGUI->CheckMouseInsideScene(mouseX, mouseY)) {
        *xpos = (mouseX - HorizontalBorder) / ScaleFactor;
        *ypos = (mouseY - 1.5 * MenuBarHeight) / ScaleFactor;
        return true;
    }
    else {
        *xpos = mouseX;
        *ypos = mouseY;
        return false;
    }
}

void EditorGUI::Inputs(GLFWwindow* window) {
    // Mouse left button

    if (_LastLeftEvent == GLFW_PRESS && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {

        // Stores the coordinates of the cursor
        double mouseX;
        double mouseY;

        if (ScaledGetCursorPos(window, &mouseX, &mouseY)) {
            // Only inside the scene window
            auto mousePosition = glm::vec2((mouseX / WindowWidth * 2) - 1, (mouseY / WindowHeight) * 2 - 1);
            std::cout << "mousePosition: " << glm::to_string(mousePosition) << std::endl;
            
            std::vector<RaycastInfo*> raycasts = GameEngine::GetInstance().RaycastFromCamera(mousePosition, 10);

            printf("Raycast results: %d\n", raycasts.size());

            for (RaycastInfo* raycast : raycasts)
            {
                std::cout << raycast->worldPoint.to_string();
                std::cout << raycast->Entity.name() << std::endl;
            }
            
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

void EditorGUI::PrintPrompt(LogEntry* entry) {
    ImGui::Begin("Test");
    ImGui::Text("Hello");
    ImGui::InputFloat4("input test", vec4f);
    if (ImGui::Button("Done")) {
        entry->IsSelected = false;
    }
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
    //flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
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
        
        Entities.push_back(GameEngine::GetInstance().ECSWorld.entity()
            .set<Transform>({ GameEngine::GetInstance().ScreenToWorldSpace(glm::vec3(0, 0, 0.95)) })
            .set<Renderer>({ Models::TEST_TEXTURE }));
        AddLogEntry(&Entities.back());
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
    flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Log", NULL, flags);    
    ImGui::PushItemWidth(LogDimensions.x - MenuBarHeight*0.4);
    ImGui::ListBoxHeader(" ", Log.size(), Log.size());
        for (auto&& entry : Log)
        {

            if (ImGui::Selectable("XXX", entry.IsSelected))
            {
                entry.IsSelected = !entry.IsSelected;
                if (entry.IsSelected)
                {
					double mouseX, mouseY;
					glfwGetCursorPos(window, &mouseX, &mouseY);
                    ImGui::SetNextWindowSize(LogEditPromptDimensions);
                    //CheckSpaceForPrompt(&mouseX, &mouseY, LogEditPromptDimensions);
					ImGui::SetNextWindowPos(LogEditPromptPositions);
                }
            }
            else {
                entry.IsSelected == false;
            }
            if (entry.IsSelected)
            {
                PrintPrompt(&entry);
			}
        }
    ImGui::ListBoxFooter();
    ImGui::End();
   
}

