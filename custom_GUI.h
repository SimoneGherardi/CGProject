#pragma once
#include "imgui.h"
#include<GLFW/glfw3.h>

struct WindowSize {
	int Width, Height;
};

void scaledGetCursorPos(GLFWwindow* window, double* xpos, double* ypos);
void showCustomWindow(ImTextureID renderTexture, WindowSize windowSize);

class EditorGUI
{
public:
	float WindowWidth;
	float WindowHeight;
    ImVec4 ClearColor;
    bool ShowDemoWindow;
    bool ShowAnotherWindow;

    // Dimensions
    float ScaleRatio;
    float MenuBarHeight;
    ImVec2 MenuBarDimensions;
    ImVec2 SceneDimensions;
    ImVec2 PrefabContainerDimensions;
    ImVec2 LogDimensions;

    // Positions
    ImVec2 MenuBarPosition;
    ImVec2 ScenePosition;
    ImVec2 PrefabContainerPosition;
    ImVec2 LogPosition;
	// Camera constructor to set up initial values
    EditorGUI(WindowSize windowSize);
    
    void showCustomWindow(ImTextureID renderTexture, WindowSize windowSize);

};
