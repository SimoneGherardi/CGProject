#include "custom_GUI.h"



EditorGUI::EditorGUI(WindowSize windowSize) : WindowWidth(windowSize.Width), WindowHeight(windowSize.Height){
    ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ShowDemoWindow = false;
    ShowAnotherWindow = true;

    // Dimensions
    ScaleFactor = 0.7f;
    MenuBarHeight = ((float)windowSize.Height) * 0.04;
    HorizontalBorder = 0.0f;
    MenuBarDimensions = ImVec2(((float)windowSize.Width), MenuBarHeight);
    SceneDimensions = ImVec2(((float)windowSize.Width) * ScaleFactor, ((float)windowSize.Height) * ScaleFactor + 0.5 * MenuBarHeight);
    PrefabContainerDimensions = ImVec2(SceneDimensions[0], ((float)windowSize.Height) - SceneDimensions.y);
    LogDimensions = ImVec2(((float)windowSize.Width) * (1.0f - ScaleFactor), ((float)windowSize.Height) - MenuBarHeight);

    // Positions
    MenuBarPosition = ImVec2(0, 0);
    ScenePosition = ImVec2(0, MenuBarHeight);
    PrefabContainerPosition = ImVec2(0, SceneDimensions[1] + MenuBarHeight);
    LogPosition = ImVec2(SceneDimensions[0], MenuBarHeight);
}


void EditorGUI::showCustomWindow(ImTextureID renderTexture, WindowSize windowSize) {
    // Menu bar
    ImGui::SetNextWindowSize(MenuBarDimensions);
    ImGui::SetNextWindowPos(MenuBarPosition);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
    ImGui::Begin("Menu Bar", NULL, flags);
    if (ImGui::Button("Open")) {}
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

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window", &ShowDemoWindow);      // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &ShowAnotherWindow);


    //ImGui::SliderFloat("float", &F, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&ClearColor); // Edit 3 floats representing a color

    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    ImGui::SameLine();

    ImGui::End();

    //Log
    ImGui::SetNextWindowSize(LogDimensions);
    ImGui::SetNextWindowPos(LogPosition);
    flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Log", NULL, flags);

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window", &ShowDemoWindow);      // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &ShowAnotherWindow);


    // ImGui::SliderFloat("float", &F, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&ClearColor); // Edit 3 floats representing a color

    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    ImGui::SameLine();

    ImGui::End();
   
}

