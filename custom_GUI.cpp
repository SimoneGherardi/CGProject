#include "custom_GUI.h"

LogEntry::LogEntry(std::string text, bool isSelected): Text(text), IsSelected(isSelected) {
}

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


void EditorGUI::addLogEntry(std::string entryText) {
    Log.push_back(LogEntry(entryText, false));
}



void EditorGUI::showCustomWindow(ImTextureID renderTexture, WindowSize windowSize) {

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
    if (ImGui::Button("Square Block")) {
        addLogEntry("Square Block");
    }
    ImGui::SameLine();
    if (ImGui::Button("T Block")) {
        addLogEntry("T Block");
    }
    ImGui::SameLine();
    if (ImGui::Button("L Block")) {
        addLogEntry("L Block");
    }
    ImGui::SameLine();
    if (ImGui::Button("Reverse L Block")) {
        addLogEntry("Reverse L Block");
    }
    ImGui::SameLine();
    if (ImGui::Button("Z Block")) {
        addLogEntry("Z Block");
    }
    ImGui::SameLine();
    if (ImGui::Button("Reverse Z Block")) {
        addLogEntry("Reverze Z Block");
    }
    ImGui::SameLine();


    ImGui::End();

    //Log
    ImGui::SetNextWindowSize(LogDimensions);
    ImGui::SetNextWindowPos(LogPosition);
    flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Log", NULL, flags);    
    /*
    ImGui::ListBoxHeader(...)
        for (item : my_vector_containing_my_items)
        {
            std::string& item_name = item.Name;
            if (ImGui::Selectable(s.c_str(), item.IsSelected))
            {
                // handle selection
            }
        }
    ImGui::ListBoxFooter()
        */
    ImGui::ListBoxHeader("", Log.size());
        for (auto&& item : Log)
        {

            if (ImGui::Selectable(item.Text.c_str(), item.IsSelected))
            {
                // handle selection
            }
        }
    ImGui::ListBoxFooter();
    ImGui::End();
   
}

