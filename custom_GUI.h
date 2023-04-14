#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include<GLFW/glfw3.h>
#include "stb_image.h"
#include <string>
#include <vector>

struct WindowSize {
	int Width, Height;
};

struct LogEntry {
    std::string Text;
    bool IsSelected;

    LogEntry(std::string text, bool isSelected);
};

class EditorGUI
{
public:
	float WindowWidth;
	float WindowHeight;
    ImVec4 ClearColor;
    bool ShowDemoWindow;
    bool ShowAnotherWindow;
    int Counter = 0;


    std::vector <LogEntry> Log;

    // Dimensions
    float ScaleFactor;
    float MenuBarHeight;
    float HorizontalBorder;
    ImVec2 MenuBarDimensions;
    ImVec2 SceneDimensions;
    ImVec2 PrefabContainerDimensions;
    ImVec2 LogDimensions;
    ImVec2 ButtonDimensions = ImVec2(200, 50);
    ImVec2 LogEntryDimensions  = ImVec2(100, 50);

    // Positions
    ImVec2 MenuBarPosition;
    ImVec2 ScenePosition;
    ImVec2 PrefabContainerPosition;
    ImVec2 LogPosition;

	
    EditorGUI(WindowSize windowSize);
    
    void showCustomWindow(ImTextureID renderTexture, WindowSize windowSize);
    void addLogEntry(std::string entryText);

};
